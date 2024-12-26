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
  const {
    cameraId,
    micId,
    cameraDimension,
    cameraFrameRate,
    screenDimension,
    screenFrameRate,
    $patch: setDeviceInfo,
  } = useDeviceInfo();
  const { publish } = useChannel();
  const updateDeviceList = (deviceType: DeviceType, info: DeviceInfo) => {
    setDeviceInfo((prev) => {
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
    const [newCameraTrack, newMicTrack] = (await DingRTC.createMicrophoneAndCameraTracks(
      { dimension: cameraDimension, frameRate: cameraFrameRate },
      {},
    )) as [CameraVideoTrack, MicrophoneAudioTrack];
    getDeviceList(newCameraTrack, newMicTrack);
    print('got camera and mic tracks');
    newCameraTrack.on('track-ended', () => {
      channelInfo.$patch({ cameraTrack: null });
    });
    newMicTrack.on('track-ended', () => {
      channelInfo.$patch({ micTrack: null });
    });
    channelInfo.$patch({
      cameraTrack: newCameraTrack,
      micTrack: newMicTrack,
    });
    return [newCameraTrack, newMicTrack];
  };

  const getDeviceList = (camera: CameraVideoTrack, mic: MicrophoneAudioTrack) => {
    if (camera) {
      DingRTC.getCameras().then((cameraList) => {
        const pattern = /\([0-9a-zA+Z:]+\)/i;
        const newCameraList = cameraList
          .filter((item) => item.deviceId)
          .map((item) => ({
            ...item.toJSON(),
            label: item.label.replace(pattern, ''),
          }));
        setDeviceInfo({
          cameraList: newCameraList,
          cameraId: camera.getMediaStreamTrack()?.getCapabilities?.()?.deviceId,
        });
      });
    }
    if (mic) {
      Promise.all([DingRTC.getMicrophones(), DingRTC.getPlaybackDevices()]).then((result) => {
        const [micList, speakerList] = result;
        const newMicList = micList.filter((item) => item.deviceId);
        const newSpeakerList = speakerList.filter((item) => item.deviceId);
        const currentMicId = mic.getMediaStreamTrack()?.getCapabilities?.()?.deviceId;
        setDeviceInfo({
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
      deviceId: cameraId,
      dimension: cameraDimension,
      frameRate: cameraFrameRate,
    }).then((track) => {
      loading.value = false;
      print('got camera track');
      getDeviceList(track, null);
      if (!cameraId) {
        const currentCameraId = track.getMediaStreamTrack()?.getCapabilities?.()?.deviceId;
        setDeviceInfo({
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
    return DingRTC.createMicrophoneAudioTrack({ deviceId: micId }).then((track) => {
      loading.value = false;
      getDeviceList(null, track);
      if (!micId) {
        const currentMicId = track.getMediaStreamTrack()?.getCapabilities()?.deviceId;
        setDeviceInfo({
          micId: currentMicId,
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
      dimension: screenDimension,
      frameRate: screenFrameRate,
    }).then((track) => {
      loading.value = false;
      print('got screen track');
      return track[0] as LocalVideoTrack;
    });
  };

  const operateCamera = () => {
    if (!channelInfo.cameraTrack) {
      openCamera().then((track) => {
        if (scene !== 'pre') {
          publish([track]);
        }
        DingRTC.getCameras().then((list) => {
          setDeviceInfo({ cameraList: list.filter((item) => item.deviceId) });
        });
      });
    } else {
      channelInfo.cameraTrack.setEnabled(!channelInfo.cameraTrack.enabled).then(() => {
        print(`cameraTrack change to ${!channelInfo.cameraTrack.enabled ? 'disbaled' : 'enabled'}`);
        channelInfo.$patch({ cameraTrack: channelInfo.cameraTrack });
      });
    }
  };

  const operateMic = () => {
    if (!channelInfo.micTrack) {
      openMic().then((track) => {
        const inPre = scene === 'pre';
        if (!inPre) publish([track]);
        DingRTC.getMicrophones().then((list) => {
          setDeviceInfo({ micList: list.filter((item) => item.deviceId) });
        });
        DingRTC.getPlaybackDevices().then((list) => {
          setDeviceInfo({ speakerList: list.filter((item) => item.deviceId) });
        });
      });
    } else {
      channelInfo.micTrack.setEnabled(!channelInfo.micTrack.enabled).then(() => {
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
    cameraDimension,
    cameraFrameRate,
    screenDimension,
    screenFrameRate,
    operateCamera,
    operateScreen,
    updateDeviceList,
    openMicAndCameraSameTime,
  };
};
