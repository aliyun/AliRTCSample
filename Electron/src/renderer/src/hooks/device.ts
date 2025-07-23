import { markRaw, ref, toRaw } from 'vue';
import { useDeviceInfo, useChannelInfo } from '~/store';
import { print } from '~/utils/tools';

import DingRTC, {
  AudioProfile,
  CameraVideoTrack,
  DeviceInfo,
  LocalVideoTrack,
  MicrophoneAudioTrack,
  PlaybackAudioTrack,
} from 'dingrtc-electron-sdk';

import { useChannel } from './channel';

type DeviceType = 'camera' | 'playback-device' | 'microphone';

export const useDevice = (scene?: 'pre' | 'in') => {
  const loading = ref(false);
  const channelInfo = useChannelInfo();
  const {
    cameraId,
    micId,
    speakerId,
    cameraDimension,
    cameraFrameRate,
    screenDimension,
    screenFrameRate,
    cameraEnable,
    micEnable,
    $patch: setDeviceInfo,
  } = useDeviceInfo();
  const { publish, unpublish } = useChannel();
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
      prev[prevListMap[deviceType].key] = newList;
    });
    if (info.state === 'inactive') {
      if (deviceType === 'microphone' && info.device.deviceId === channelInfo.micTrack?.deviceId) {
        // current mic was removed
        DingRTC.getMicrophones().then((list) => {
          if (list.length > 0) {
            channelInfo.micTrack?.setDevice(list[0].deviceId);
            setDeviceInfo({
              micId: channelInfo.micTrack?.deviceId
            });
          }
        });
      } else if (deviceType === 'playback-device' && info.device.deviceId === channelInfo.speakerTrack?.deviceId) {
        // current speaker was removed
        DingRTC.getPlaybackDevices().then((list) => {
          if (list.length > 0) {
            channelInfo.speakerTrack?.setDevice(list[0].deviceId);
            setDeviceInfo({
              speakerId: channelInfo.speakerTrack?.deviceId
            });
          }
        });
      } else if (deviceType === 'camera' && info.device.deviceId === channelInfo.cameraTrack?.deviceId) {
        // current camera was removed
        DingRTC.getCameras().then((list) => {
          if (list.length > 0) {
            channelInfo.cameraTrack?.setDevice(list[0].deviceId);
            setDeviceInfo({
              cameraId: channelInfo.cameraTrack?.deviceId
            });
          }
        });
      }
    }
  };

  const openMicAndCameraSameTime = async () => {
    const [newCameraTrack, newMicTrack] = (await DingRTC.createMicrophoneAndCameraTracks(
      { dimension: cameraDimension, frameRate: cameraFrameRate },
      { profile: AudioProfile.Basic },
    )) as [CameraVideoTrack, MicrophoneAudioTrack];
    const speakerTrack = await DingRTC.createPlaybackAudioTrack();
    getDeviceList(newCameraTrack, newMicTrack, speakerTrack);
    print('got camera and mic tracks');
    newCameraTrack.on('track-ended', () => {
      channelInfo.$patch({ cameraTrack: null });
    });
    newMicTrack.on('track-ended', () => {
      channelInfo.$patch({ micTrack: null });
    });
    setDeviceInfo({ cameraEnable: newCameraTrack.enabled, micEnable: !newMicTrack.muted });
    channelInfo.$patch({
      cameraTrack: markRaw(newCameraTrack),
      micTrack: markRaw(newMicTrack),
      speakerTrack: markRaw(speakerTrack),
    });
    if (cameraEnable != channelInfo.cameraTrack.enabled) {
      operateCamera();
    }
    if (micEnable != !channelInfo.micTrack.muted) {
      operateMic();
    }
    return newCameraTrack;
  };

  const getDeviceList = (
    camera: CameraVideoTrack,
    mic: MicrophoneAudioTrack,
    speaker: PlaybackAudioTrack,
  ) => {
    if (camera) {
      DingRTC.getCameras().then((cameraList) => {
        const pattern = /\([0-9a-zA+Z:]+\)/i;
        const newCameraList = cameraList
          .filter((item) => item.deviceId)
          .map((item) => ({
            ...item,
            label: item.label.replace(pattern, ''),
          }));
        setDeviceInfo({
          cameraList: newCameraList,
          cameraId: camera?.deviceId,
        });
      });
    }
    if (mic) {
      Promise.all([DingRTC.getMicrophones(), DingRTC.getPlaybackDevices()]).then((result) => {
        const [micList, speakerList] = result;
        const newMicList = micList.filter((item) => item.deviceId);
        const newSpeakerList = speakerList.filter((item) => item.deviceId);
        setDeviceInfo({
          micList: newMicList,
          speakerList: newSpeakerList,
          micId: mic?.deviceId,
          speakerId: speaker?.deviceId,
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
      getDeviceList(track, null, null);
      if (!cameraId) {
        const currentCameraId = track?.deviceId;
        setDeviceInfo({
          cameraId: currentCameraId,
        });
      }
      setDeviceInfo({ cameraEnable: track.enabled });
      track.on('track-ended', () => {
        channelInfo.$patch({ cameraTrack: null });
      });
      channelInfo.$patch({ cameraTrack: track });
      return track;
    });
  };

  const openMic = async () => {
    const micTrack = await DingRTC.createMicrophoneAudioTrack({ deviceId: micId });
    const speakerTrack = await DingRTC.createPlaybackAudioTrack({ deviceId: speakerId });
    loading.value = false;
    getDeviceList(null, micTrack, speakerTrack);
    if (!micId) {
      const currentMicId = micTrack?.deviceId;
      setDeviceInfo({
        micId: currentMicId,
        speakerId: speakerTrack.deviceId,
      });
    }
    micTrack.on('track-ended', () => {
      channelInfo.$patch({ micTrack: null });
    });
    setDeviceInfo({ micEnable: !micTrack.muted });
    channelInfo.$patch({ micTrack: markRaw(micTrack), speakerTrack: markRaw(speakerTrack) });
    print('got mic track');
    return micTrack;
  };

  const openScreen = (type: 'window' | 'screen', sourceId: string) => {
    if (loading.value) return Promise.reject();
    return DingRTC.createScreenVideoTrack({
      dimension: screenDimension,
      frameRate: screenFrameRate,
      type,
      sourceId,
    }).then((track) => {
      loading.value = false;
      print('got screen track');
      return track as LocalVideoTrack;
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
      const newEnabled = !channelInfo.cameraTrack.enabled;
      toRaw(channelInfo.cameraTrack)
        .setEnabled(newEnabled)
        .then(() => {
          print(`cameraTrack change to ${!newEnabled ? 'disbaled' : 'enabled'}`);
          channelInfo.$patch({ cameraTrack: markRaw(channelInfo.cameraTrack) });
          setDeviceInfo({ cameraEnable: newEnabled });
          channelInfo.updateTrackStats();
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
      const newMuted = !channelInfo.micTrack.muted;
      toRaw(channelInfo.micTrack)
        .setMuted(newMuted)
        .then(() => {
          print(`micTrack change to ${newMuted ? 'disbaled' : 'enabled'}`);
          setDeviceInfo({ micEnable: !newMuted });
          channelInfo.updateTrackStats();
        });
    }
  };

  const operateScreen = (type?: 'window' | 'screen', sourceId?: string) => {
    if (!channelInfo.screenTrack) {
      openScreen(type, sourceId).then((track) => {
        channelInfo.$patch({ screenTrack: track });
        publish([track]).catch(() => {
          track.close();
          channelInfo.$patch({ screenTrack: null });
        });
      });
    } else {
      unpublish([channelInfo.screenTrack as LocalVideoTrack]);
      toRaw(channelInfo.screenTrack)?.close();
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
