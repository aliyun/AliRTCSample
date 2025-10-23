import { useDeviceInfo, useChannelInfo } from '~/store';
import DingRTC, {
  CameraVideoTrack,
  DeviceInfo,
  LocalVideoTrack,
  MicrophoneAudioTrack,
} from 'dingrtc';
import { logger, parseSearch } from '~/utils/tools';
import { ref } from 'vue';
import { useChannel } from './channel';
import { isMobile } from '~/utils/tools';
// @ts-ignore
window.DingRTC = DingRTC;

type DeviceType = 'camera' | 'playback-device' | 'microphone';

export const useDevice = (scene?: 'pre' | 'in') => {
  const loading = ref(false);
  const channelInfo = useChannelInfo();
  const deviceInfo = useDeviceInfo();
  const { publish } = useChannel();
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

  const openScreen = () => {
    if (loading.value) return Promise.reject();
    return DingRTC.createScreenVideoTrack({
      dimension: deviceInfo.screenDimension,
      frameRate: 17, //deviceInfo.screenFrameRate,
      optimizationMode: 'detail',
    }).then((track) => {
      loading.value = false;
      logger.info('got screen track');
      return track[0] as LocalVideoTrack;
    });
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
    if (!channelInfo.micTrack) {
      return openMic().then((track) => {
        const inPre = scene === 'pre';
        if (!inPre) publish([track]);
      });
    } else {
      return channelInfo.micTrack.setEnabled(!channelInfo.micTrack.enabled).then(() => {
        logger.info(`micTrack change to ${!channelInfo.micTrack.enabled ? 'disbaled' : 'enabled'}`);
      });
    }
  };

  const operateScreen = () => {
    if (!channelInfo.screenTrack) {
      openScreen().then((track) => {
        channelInfo.$patch({ screenTrack: track });
        publish([track]).catch(() => {
          track.close();
          channelInfo.$patch({ screenTrack: null });
        });
      });
    } else {
      channelInfo.screenTrack?.close();
      logger.info(`stop share screen`);
      channelInfo.$patch({ screenTrack: null });
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
  };
};
