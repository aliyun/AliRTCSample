import { useDeviceInfo, useChannelInfo } from '~/store';
import DingRTC, {
  CameraVideoTrack,
  DeviceInfo,
  LocalVideoTrack,
  MicrophoneAudioTrack,
} from 'dingrtc';
import { print } from '~/utils/tools';
import { ref } from 'vue';
import { useChannel } from './channel';

type DeviceType = 'camera' | 'playback-device' | 'microphone';

export const useDevice = (scene?: 'pre' | 'in') => {
  const loading = ref(false);
  const  channelInfo = useChannelInfo()
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
      print(`${deviceType} ${info.state}`, info.device.label, info.device.deviceId);
      const index = prevList.findIndex(
        (item: MediaDeviceInfo) => item.deviceId === info.device.deviceId,
      );
      if (info.state === 'active' && index === -1) {
        newList.push(info.device);
      } else if (info.state === 'inactive' && index !== -1) {
        newList.splice(index, 1);
      }
      return {
        [prevListMap[deviceType].key]: newList,
      };
    });
  };

  const openMicAndCameraSameTime = async () => {
    await getDeviceList(true, true);
    const [newCameraTrack, newMicTrack] = (await DingRTC.createMicrophoneAndCameraTracks(
      { dimension: deviceInfo.cameraDimension, frameRate: deviceInfo.cameraFrameRate, deviceId: deviceInfo.cameraId },
      { deviceId: deviceInfo.micId },
    )) as [CameraVideoTrack, MicrophoneAudioTrack];
    print('got camera and mic tracks');
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
    })
    channelInfo.$patch({
      cameraTrack: newCameraTrack,
      micTrack: newMicTrack,
    });
    return [newCameraTrack, newMicTrack];
  };

  const getDeviceList = async (camera: boolean, mic: boolean) => {
    if (camera) {
      await DingRTC.getCameras().then((cameraList) => {
        const pattern = /\([0-9a-zA+Z:]+\)/i;
        const newCameraList = cameraList
          .filter((item) => item.deviceId)
          .map((item) => ({
            ...item.toJSON(),
            label: item.label.replace(pattern, ''),
          }));
        deviceInfo.cameraList = newCameraList;
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
    return DingRTC.createCameraVideoTrack({
      deviceId: deviceInfo.cameraId,
      dimension: deviceInfo.cameraDimension,
      frameRate: deviceInfo.cameraFrameRate,
    }).then((track) => {
      loading.value = false;
      print('got camera track');
      if (!deviceInfo.cameraId) {
        const currentCameraId = track.getMediaStreamTrack()?.getCapabilities?.()?.deviceId;
        deviceInfo.$patch({
          cameraId: currentCameraId,
        });
      }
      track.on('track-ended', () => {
        channelInfo.$patch({ cameraTrack: null });
      });
      channelInfo.$patch({ cameraTrack: track });
      return track;
    });
  };

  const openMic = () => {
    return DingRTC.createMicrophoneAudioTrack({ deviceId: deviceInfo.micId }).then((track) => {
      loading.value = false;
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
      print('got mic track');
      return track;
    });
  };

  const openScreen = () => {
    if (loading.value) return Promise.reject();
    return DingRTC.createScreenVideoTrack({
      dimension: deviceInfo.screenDimension,
      frameRate: deviceInfo.screenFrameRate,
    }).then((track) => {
      loading.value = false;
      print('got screen track');
      return track[0] as LocalVideoTrack;
    });
  };

  const operateCamera = () => {
    if (!channelInfo.cameraTrack) {
      return openCamera().then((track) => {
        if (scene !== 'pre') publish([track]);
      });
    } else {
      return channelInfo.cameraTrack.setEnabled(!channelInfo.cameraTrack.enabled).then(() => {
        print(`cameraTrack change to ${!channelInfo.cameraTrack.enabled ? 'disbaled' : 'enabled'}`);
        channelInfo.$patch({ cameraTrack: channelInfo.cameraTrack });
      });
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
        print(`micTrack change to ${!channelInfo.micTrack.enabled ? 'disbaled' : 'enabled'}`);
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
      print(`stop share screen`);
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
