import DingRTC, {
  CameraVideoTrack,
  DeviceInfo,
  LocalVideoTrack,
  MicrophoneAudioTrack,
} from 'dingrtc';
import { computed, ref } from 'vue';

import { useDeviceInfoStore } from '@src/store';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { DeviceType } from '@src/types/types';
import { print } from '@src/utils/tools';

import { useLocalChannel } from './useLocalChannel';

export const useDevice = (scene?: 'pre' | 'in') => {
  const loading = ref(false);
  const deviceInfoStore = useDeviceInfoStore();
  const localInfoStore = useLocalChannelInfoStore();
  const { publish } = useLocalChannel();
  /**
   * 更新设备列表
   * @param deviceType 设备类型
   * @param info 设备信息
   */
  const updateDeviceList = (deviceType: DeviceType, info: DeviceInfo) => {
    if (deviceType === 'camera') {
      deviceInfoStore.updateCameraList(info);
    } else if (deviceType === 'microphone') {
      deviceInfoStore.updateMicrophoneList(info);
    } else {
      deviceInfoStore.updateSpeakerList(info);
    }
  };

  /**
   * 同时开启摄像头和麦克风
   */
  const openMicAndCameraSameTime = async () => {
    const [newCameraTrack, newMicTrack] = (await DingRTC.createMicrophoneAndCameraTracks(
      {
        dimension: deviceInfoStore.cameraDimension,
        frameRate: deviceInfoStore.cameraFrameRate,
      },
      {},
    )) as [CameraVideoTrack, MicrophoneAudioTrack];
    _getDeviceList({
      camera: newCameraTrack,
      mic: newMicTrack,
    });
    print('got camera and mic tracks');
    newCameraTrack.on('track-ended', () => {
      localInfoStore.$patch({
        cameraTrack: undefined,
      });
    });
    newMicTrack.on('track-ended', () => {
      localInfoStore.$patch({
        micTrack: undefined,
      });
    });
    localInfoStore.$patch({
      cameraTrack: newCameraTrack,
      micTrack: newMicTrack,
    });
    return [newCameraTrack, newMicTrack];
  };

  /**
   * 获取设备列表
   */
  const _getDeviceList: (props: {
    camera?: CameraVideoTrack;
    mic?: MicrophoneAudioTrack;
  }) => void = ({ camera, mic }) => {
    if (camera) {
      DingRTC.getCameras().then((cameraList) => {
        const pattern = /\([0-9a-zA+Z:]+\)/i;
        const newCameraList = cameraList
          .filter((item) => item.deviceId)
          .map((item) => ({
            ...item.toJSON(),
            label: item.label.replace(pattern, ''),
          }));
        deviceInfoStore.$patch({
          cameraList: newCameraList,
          cameraId: camera.getMediaStreamTrack().getCapabilities().deviceId,
        });
      });
    }
    if (mic) {
      Promise.all([DingRTC.getMicrophones(), DingRTC.getPlaybackDevices()]).then((result) => {
        const [micList, speakerList] = result;
        const newMicList = micList.filter((item) => item.deviceId);
        const newSpeakerList = speakerList.filter((item) => item.deviceId);
        const currentMicId = mic.getMediaStreamTrack()?.getCapabilities()?.deviceId;
        deviceInfoStore.$patch({
          micList: newMicList,
          speakerList: newSpeakerList,
          micId: currentMicId,
          speakerId: newSpeakerList?.[0]?.deviceId,
        });
      });
    }
  };

  const openCamera = () => {
    return DingRTC.createCameraVideoTrack({
      deviceId: deviceInfoStore.cameraId,
      dimension: deviceInfoStore.cameraDimension,
      frameRate: deviceInfoStore.cameraFrameRate,
    }).then((track) => {
      loading.value = false;
      print('got camera track');
      _getDeviceList({
        camera: localInfoStore.cameraTrack as CameraVideoTrack,
      });
      track.on('track-ended', () => {
        localInfoStore.$patch({ cameraTrack: undefined });
      });
      localInfoStore.$patch({ cameraTrack: track });
      return track;
    });
  };

  const openMic = () => {
    return DingRTC.createMicrophoneAudioTrack({
      deviceId: deviceInfoStore.micId,
    }).then((track) => {
      loading.value = false;
      // 同上
      _getDeviceList({
        mic: localInfoStore.micTrack as MicrophoneAudioTrack,
      });
      track.on('track-ended', () => {
        localInfoStore.$patch({
          micTrack: undefined,
        });
      });
      localInfoStore.$patch({ micTrack: track });
      print('got mic track');
      return track;
    });
  };

  const openScreen = () => {
    if (loading.value) return Promise.reject();
    const { screenDimension, screenFrameRate } = deviceInfoStore;
    return DingRTC.createScreenVideoTrack({
      dimension: screenDimension,
      frameRate: screenFrameRate,
    }).then((track) => {
      localInfoStore.$patch({
        screenTrack: track[0],
      });
      track[0].on('track-ended', () => {
        localInfoStore.$patch({
          screenTrack: undefined,
        });
      });
      loading.value = false;
      print('got screen track');
      return track[0] as LocalVideoTrack;
    });
  };

  const operateCamera = () => {
    const cameraTrack = localInfoStore.cameraTrack;
    const cameraList = deviceInfoStore.cameraList;
    if (!cameraTrack) {
      openCamera().then((track) => {
        const inPre = scene === 'pre';
        if (!inPre) {
          publish([track]);
        }
        DingRTC.getCameras().then((list) => {
          deviceInfoStore.$patch({
            cameraList: list.filter((item) => item.deviceId),
          });
        });
      });
    } else {
      (cameraTrack as CameraVideoTrack).setEnabled(!cameraTrack.enabled).then(() => {
        print(`cameraTrack change to ${!cameraTrack?.enabled ? 'disbaled' : 'enabled'}`);
        deviceInfoStore.$patch({
          cameraList: cameraList as MediaDeviceInfo[],
        });
      });
    }
  };

  const operateMic = () => {
    if (!localInfoStore.micTrack) {
      openMic().then((track) => {
        const inPre = scene === 'pre';
        if (!inPre) publish([track]);
        DingRTC.getMicrophones().then((list) => {
          deviceInfoStore.$patch({
            micList: list.filter((item) => item.deviceId),
          });
        });
        DingRTC.getPlaybackDevices().then((list) => {
          deviceInfoStore.$patch({
            speakerList: list.filter((item) => item.deviceId),
          });
        });
      });
    } else {
      localInfoStore.micTrack?.setEnabled(!localInfoStore.micTrack.enabled).then(() => {
        print(`micTrack change to ${!localInfoStore.micTrack?.enabled ? 'disbaled' : 'enabled'}`);
      });
    }
  };

  const operateScreen = () => {
    if (!localInfoStore.screenTrack) {
      openScreen().then((track) => {
        localInfoStore.$patch({ screenTrack: track });
        const inPre = scene === 'pre';
        if (!inPre) {
          publish([track]).catch(() => {
            track.close();
            localInfoStore.$patch({
              screenTrack: undefined,
            });
          });
        }
        track.on('track-ended', () => {
          localInfoStore.$patch({ screenTrack: undefined });
        });
      });
    } else {
      localInfoStore.screenTrack?.close();
      print(`stop share screen`);
      localInfoStore.$patch({
        screenTrack: undefined,
      });
    }
  };

  /**
   *  初始化设备列表
   */
  const initDeviceList: (props: { camera?: boolean; mic?: boolean }) => void = ({
    camera,
    mic,
  }) => {
    if (camera) {
      DingRTC.getCameras().then((cameraList) => {
        const pattern = /\([0-9a-zA+Z:]+\)/i;
        const newCameraList = cameraList
          .filter((item) => item.deviceId)
          .map((item) => ({
            ...item.toJSON(),
            label: item.label.replace(pattern, ''),
          }));
        deviceInfoStore.$patch({ cameraList: newCameraList });
        if (!deviceInfoStore.cameraId) {
          deviceInfoStore.$patch({ cameraId: newCameraList[0]?.deviceId });
        }
      });
    }
    if (mic) {
      Promise.all([DingRTC.getMicrophones(), DingRTC.getPlaybackDevices()]).then((result) => {
        const [micList, speakerList] = result;
        const newMicList = micList.filter((item) => item.deviceId);
        const newSpeakerList = speakerList.filter((item) => item.deviceId);
        deviceInfoStore.$patch({
          micList: newMicList,
          speakerList: newSpeakerList,
        });
        if (!deviceInfoStore.micId) {
          deviceInfoStore.$patch({
            micId: newMicList[0]?.deviceId,
            speakerId: newSpeakerList[0]?.deviceId,
          });
        }
      });
    }
  };

  const cameraEnabled = computed(() => localInfoStore.cameraTrack?.enabled);
  const micEnabled = computed(() => localInfoStore.micTrack?.enabled);

  return {
    updateDeviceList,
    openMicAndCameraSameTime,
    openCamera,
    openMic,
    openScreen,
    operateCamera,
    cameraEnabled,
    micEnabled,
    operateMic,
    operateScreen,
    initDeviceList,
  };
};
