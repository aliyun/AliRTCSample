import { useDeviceInfo, useChannelInfo, useCurrentUserInfo } from '~/store';
import DingRTC, {
  CameraVideoTrack,
  DeviceInfo,
  LocalAudioTrack,
  LocalVideoTrack,
  MicrophoneAudioTrack,
} from 'dingrtc';
import { logger, parseSearch } from '~/utils/tools';
import { ref, toRaw } from 'vue';
import { useChannel } from './channel';
import { isIOS, isMobile } from '~/utils/tools';
// @ts-ignore
window.DingRTC = DingRTC;

type DeviceType = 'camera' | 'playback-device' | 'microphone';

const MIC_REBUILD_BACKOFFS = [0, 300, 1000];

// 模块级：useDevice 每次调用都会新建实例，重入保护必须跨组件共享
const micRecovering = ref(false);

const delay = (ms: number) => new Promise((resolve) => setTimeout(resolve, ms));

/**
 * SDK 的 LocalAudioTrack.getAudioTrack() 在 getUserMedia 失败时会静默降级成
 * createOscillator + createMediaStreamDestination 合成的无声轨道，且不抛错。
 * 合成轨并非真实采集设备，其 deviceId 不会出现在 enumerateDevices 的 audioinput
 * 列表中（Chrome 下形如 WebAudio-xxx），据此与真实麦克风轨区分。
 */
const isSilentFallbackTrack = async (track?: LocalAudioTrack) => {
  const deviceId = track?.getMediaStreamTrack()?.getSettings?.().deviceId;
  if (!deviceId) return false;
  const inputs = (await navigator.mediaDevices.enumerateDevices()).filter(
    (item) => item.kind === 'audioinput' && item.deviceId,
  );
  // 设备列表不可用时放弃判断，避免误判导致每次开麦都重建
  if (!inputs.length) return false;
  return !inputs.some((item) => item.deviceId === deviceId);
};

export const useDevice = (scene?: 'pre' | 'in') => {
  const loading = ref(false);
  const channelInfo = useChannelInfo();
  const deviceInfo = useDeviceInfo();
  const currentUserInfo = useCurrentUserInfo();
  const { publish, unpublish } = useChannel();
  const updateDeviceList = (deviceType: DeviceType, info: DeviceInfo) => {
    deviceInfo.$patch((prev) => {
      const { cameraList, micList, speakerList } = prev;
      const prevListMap: Record<DeviceType, any> = {
        camera: { key: 'cameraList', value: cameraList },
        'playback-device': { key: 'speakerList', value: speakerList },
        microphone: { key: 'micList', value: micList },
      };
      const prevList = prevListMap[deviceType].value;
      const newList = [...prevList];
      logger.info(`${deviceType} ${info.state}`, info.device.label, info.device.deviceId);
      const index = prevList.findIndex(
        (item: MediaDeviceInfo) => item.deviceId === info.device.deviceId,
      );
      if (info.state === 'active' && index === -1) {
        newList.push(info.device);
      } else if (info.state === 'inactive' && index !== -1) {
        newList.splice(index, 1);
      }
      return {
        [prevListMap[deviceType].key]: [...newList],
      };
    });
  };

  const openMicAndCameraSameTime = async () => {
    await getDeviceList(true, true);

    let newMicTrack;
    let newCameraTrack;
    const isUseCustomAudioTrack = parseSearch('customAudioApi') === 'true';
    if (isUseCustomAudioTrack) {
      newCameraTrack = await DingRTC.createCameraVideoTrack({
        dimension: deviceInfo.cameraDimension,
        frameRate: 17, //deviceInfo.cameraFrameRate,
        deviceId: deviceInfo.cameraId,
        optimizationMode: 'detail',
      });
      const audioStream = await navigator.mediaDevices.getUserMedia({
        audio: { deviceId: deviceInfo.micId },
        video: false,
      });
      newMicTrack = await DingRTC.createCustomAudioTrack({
        mediaStreamTrack: audioStream.getAudioTracks()[0],
      });
    } else {
      [newCameraTrack, newMicTrack] = (await DingRTC.createMicrophoneAndCameraTracks(
        {
          dimension: deviceInfo.cameraDimension,
          frameRate: 17, //deviceInfo.cameraFrameRate,
          deviceId: deviceInfo.cameraId,
          optimizationMode: 'detail',
        },
        { deviceId: deviceInfo.micId },
      )) as [CameraVideoTrack, MicrophoneAudioTrack];
    }

    logger.info('got camera and mic tracks');
    // @ts-ignore
    window.newCameraTrack = newCameraTrack;
    newCameraTrack.on('track-ended', () => {
      channelInfo.$patch({ cameraTrack: null });
    });
    newMicTrack.on('track-ended', () => {
      channelInfo.$patch({ micTrack: null });
    });
    deviceInfo.$patch({
      cameraId: newCameraTrack?.getMediaStreamTrack()?.getSettings().deviceId,
      micId: newMicTrack?.getMediaStreamTrack()?.getSettings().deviceId,
      speakerId: deviceInfo.speakerList?.[0]?.deviceId,
    });
    channelInfo.$patch({
      cameraTrack: newCameraTrack,
      micTrack: newMicTrack,
    });
    return [newCameraTrack, newMicTrack];
  };

  const getDeviceList = async (camera: boolean, mic: boolean) => {
    if (camera) {
      await DingRTC.getCameras().then((cameraList) => {
        logger.info(cameraList);
        const pattern = /\([0-9a-zA+Z:]+\)/i;
        const newCameraList = cameraList
          .filter((item) => item.deviceId)
          .map((item) => ({
            ...item.toJSON(),
            label: item.label.replace(pattern, ''),
          }));
        if (isMobile()) {
          logger.info('isMobile', isMobile());
          // mock一下前后置摄像头
          deviceInfo.cameraList = [
            {
              deviceId: '1',
              groupId: 'mock',
              kind: 'videoinput',
              label: 'user',
              toJSON: function () {
                throw new Error('demo mock device obj.');
              },
            },
            {
              deviceId: '0',
              groupId: '',
              kind: 'videoinput',
              label: 'environment',
              toJSON: function () {
                throw new Error('demo mock device obj.');
              },
            },
          ];
        } else {
          deviceInfo.cameraList = newCameraList;
        }
      });
    }
    if (mic) {
      await Promise.all([DingRTC.getMicrophones(), DingRTC.getPlaybackDevices()]).then((result) => {
        const [micList, speakerList] = result;
        const newMicList = micList.filter((item) => item.deviceId);
        const newSpeakerList = speakerList.filter((item) => item.deviceId);
        deviceInfo.$patch({
          micList: newMicList,
          speakerList: newSpeakerList,
        });
      });
    }
  };

  const openCamera = () => {
    const videoConstraints = {
      deviceId: deviceInfo.cameraId,
      dimension: deviceInfo.cameraDimension,
      frameRate: 17, //deviceInfo.cameraFrameRate,
    };
    if (isMobile()) {
      delete videoConstraints.deviceId;
      // @ts-ignore
      videoConstraints.facingMode = 'user';
    }
    return DingRTC.createCameraVideoTrack(videoConstraints).then((track) => {
      loading.value = false;
      logger.info('got camera track');
      if (!deviceInfo.cameraId) {
        const currentCameraId = track.getMediaStreamTrack()?.getCapabilities?.()?.deviceId;
        deviceInfo.$patch({
          cameraId: currentCameraId,
        });
      }
      getDeviceList(true, false);
      track.on('track-ended', () => {
        channelInfo.$patch({ cameraTrack: null });
      });
      channelInfo.$patch({ cameraTrack: track });
      return track;
    });
  };

  const openMic = async () => {
    let track;
    const isUseCustomAudioTrack = parseSearch('customAudioApi') === 'true';
    if (isUseCustomAudioTrack) {
      const audioStream = await navigator.mediaDevices.getUserMedia({
        audio: { deviceId: { ideal: ['default'] } },
        video: false,
      });
      track = await DingRTC.createCustomAudioTrack({
        mediaStreamTrack: audioStream.getAudioTracks()[0],
      });
    } else {
      track = await DingRTC.createMicrophoneAudioTrack({ deviceId: deviceInfo.micId });
    }

    loading.value = false;
    getDeviceList(false, true);
    if (!deviceInfo.micId) {
      const currentMicId = track.getMediaStreamTrack()?.getCapabilities()?.deviceId;
      deviceInfo.$patch({
        micId: currentMicId,
        speakerId: deviceInfo.speakerList[0]?.deviceId,
      });
    }
    track.on('track-ended', () => {
      channelInfo.$patch({ micTrack: null });
    });
    channelInfo.$patch({ micTrack: track });
    logger.info('got mic track');
    return track;
  };

  const openScreen = async () => {
    if (loading.value) return Promise.reject();
    const tracks = await DingRTC.createScreenVideoAndAudioTrack({
      dimension: deviceInfo.screenDimension,
      frameRate: 17, //deviceInfo.screenFrameRate,
      optimizationMode: 'detail',
    });
    loading.value = false;
    logger.info('got screen track', tracks);
    return tracks as LocalVideoTrack[];
  };

  /**
   * 完全重建麦克风轨道。必须走 openMic（createMicrophoneAudioTrack）而非
   * setEnabled(true) / setDevice()，后两者内部的 updateNewTrack 会在采集失败时
   * 静默返回无声合成轨；同时新建 LocalTrack 会产生新的 RTCRtpSender，
   * 可规避 iOS 上 replaceTrack 后 sender 仍持续发静音的问题。
   */
  const rebuildMicTrack = async (shouldPublish: boolean) => {
    if (micRecovering.value) return;
    micRecovering.value = true;
    logger.info(`start rebuilding micTrack, shouldPublish=${shouldPublish}`);
    try {
      const staleTrack = channelInfo.micTrack;
      if (staleTrack) {
        if (shouldPublish) {
          // 取消发布失败不应阻断重新采集；成功时 unpublish 内部会移除发布记账
          await unpublish([staleTrack]).catch((e) => {
            logger.error('unpublish stale micTrack failed, continue anyway', e);
          });
        }
        toRaw(staleTrack).close();
        channelInfo.$patch({ micTrack: null });
      }

      for (let attempt = 0; attempt < MIC_REBUILD_BACKOFFS.length; attempt++) {
        if (attempt > 0) {
          await delay(MIC_REBUILD_BACKOFFS[attempt]);
          // 后台恢复后原 deviceId 可能已失效，清空让浏览器回落到默认设备
          deviceInfo.$patch({ micId: '' });
        }
        try {
          const freshTrack = await openMic();
          if (shouldPublish) await publish([freshTrack]);
          channelInfo.updateTrackStats(currentUserInfo.userId);
          logger.info(`micTrack rebuilt at attempt ${attempt + 1}`);
          return;
        } catch (e) {
          logger.error(`rebuild micTrack attempt ${attempt + 1} failed`, e);
        }
      }

      logger.error('rebuild micTrack gave up, falling back to mic-off state');
      channelInfo.$patch({ micTrack: null });
      channelInfo.updateTrackStats(currentUserInfo.userId);
    } finally {
      micRecovering.value = false;
    }
  };

  const operateCamera = () => {
    if (!channelInfo.cameraTrack) {
      logger.info('start openCamera');
      const p = openCamera().then((track) => {
        logger.info('openCamera success');
        if (scene !== 'pre') publish([track]);
      });
      return p;
    } else {
      const p = channelInfo.cameraTrack.setEnabled(!channelInfo.cameraTrack.enabled).then(() => {
        logger.info(
          `cameraTrack change to ${!channelInfo.cameraTrack.enabled ? 'disbaled' : 'enabled'}`,
        );
        channelInfo.$patch({ cameraTrack: channelInfo.cameraTrack });
      });
      return p;
    }
  };

  const operateMic = () => {
    if (micRecovering.value) return Promise.resolve();
    if (!channelInfo.micTrack) {
      return openMic().then((track) => {
        const inPre = scene === 'pre';
        if (!inPre) publish([track]);
      });
    }
    const nextEnabled = !channelInfo.micTrack.enabled;
    const wasPublished = channelInfo.publishedTracks.has(channelInfo.micTrack.getTrackId());
    return channelInfo.micTrack.setEnabled(nextEnabled).then(async () => {
      logger.info(`micTrack change to ${nextEnabled ? 'enabled' : 'disbaled'}`);
      // setEnabled(true) 内部走 updateNewTrack，采集失败时会静默拿到无声合成轨
      if (nextEnabled && isIOS() && (await isSilentFallbackTrack(channelInfo.micTrack))) {
        logger.error('got silent fallback track on unmute, rebuilding micTrack');
        await rebuildMicTrack(wasPublished);
      }
    });
  };

  const operateScreen = async () => {
    if (!channelInfo.screenVideoTrack) {
      const tracks = await openScreen();
      channelInfo.$patch({ screenVideoTrack: tracks[0] });
      // tracks[0].on('track-ended', () => {
      //   channelInfo.$patch({ screenVideoTrack: null });
      // });
      if (tracks[1]) {
        channelInfo.$patch({ screenAudioTrack: tracks[1] });
        // tracks[1].on('track-ended', () => {
        //   channelInfo.$patch({ screenAudioTrack: null });
        // });
      }
      publish(tracks).catch(() => {
        tracks[0]?.close();
        tracks[1]?.close();
        channelInfo.$patch({ screenVideoTrack: null, screenAudioTrack: null });
      });
    } else {
      channelInfo.screenVideoTrack?.close();
      channelInfo.screenAudioTrack?.close();
      logger.info(`stop share screen`);
      channelInfo.$patch({ screenVideoTrack: null });
      channelInfo.$patch({ screenAudioTrack: null });
    }
  };

  return {
    openMic,
    openCamera,
    operateMic,
    operateCamera,
    operateScreen,
    getDeviceList,
    updateDeviceList,
    openMicAndCameraSameTime,
    rebuildMicTrack,
  };
};
