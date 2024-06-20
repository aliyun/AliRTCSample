import { useCallback, useMemo, useState } from 'react';
import { useRecoilState, useRecoilValue, useSetRecoilState } from 'recoil';
import { currentUserInfo, deviceInfo, localChannelInfo, smallViewTrackMap } from '~/store';
import DingRTC, {
  CameraVideoTrack,
  DeviceInfo,
  LocalVideoTrack,
  MicrophoneAudioTrack,
} from 'dingrtc';
import { print } from '~/utils/tools';
import { useLocalChannel } from './channel';

type DeviceType = 'camera' | 'playback-device' | 'microphone';

export const useDevice = (scene?: 'pre' | 'in') => {
  const [loading, setLoading] = useState(false);
  const [{ cameraTrack, micTrack, screenTrack }, setLocalChannelInfo] =
    useRecoilState(localChannelInfo);
  const [
    { cameraId, micId, cameraDimension, cameraFrameRate, screenDimension, screenFrameRate, cameraMaxBitrate, screenMaxBitrate },
    setDeviceInfo,
  ] = useRecoilState(deviceInfo);
  const { userId } = useRecoilValue(currentUserInfo);
  const { publish } = useLocalChannel();
  const setSmallViewMap = useSetRecoilState(smallViewTrackMap);
  const updateDeviceList = useCallback((deviceType: DeviceType, info: DeviceInfo) => {
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
        ...prev,
        [prevListMap[deviceType].key]: newList,
      };
    });
  }, []);

  const openMicAndCameraSameTime = useCallback(async () => {
    const [newCameraTrack, newMicTrack] = (await DingRTC.createMicrophoneAndCameraTracks(
      { dimension: cameraDimension, frameRate: cameraFrameRate },
      {},
    )) as [CameraVideoTrack, MicrophoneAudioTrack];
    getDeviceList(newCameraTrack, newMicTrack);
    print('got camera and mic tracks');
    newCameraTrack.on('track-ended', () => {
      setLocalChannelInfo((prev) => ({ ...prev, cameraTrack: null }));
    });
    newMicTrack.on('track-ended', () => {
      setLocalChannelInfo((prev) => ({ ...prev, micTrack: null }));
    });
    setLocalChannelInfo((prev) => ({
      ...prev,
      cameraTrack: newCameraTrack,
      micTrack: newMicTrack,
    }));
    return [newCameraTrack, newMicTrack];
  }, []);

  const getDeviceList = useCallback(
    (camera: CameraVideoTrack, mic: MicrophoneAudioTrack) => {
      if (camera) {
        DingRTC.getCameras().then((cameraList) => {
          const pattern = /\([0-9a-zA+Z:]+\)/i;
          const newCameraList = cameraList
            .filter((item) => item.deviceId)
            .map((item) => ({
              ...item.toJSON(),
              label: item.label.replace(pattern, ''),
            }));
          setDeviceInfo((prev) => ({
            ...prev,
            cameraList: newCameraList,
            cameraId: camera.getMediaStreamTrack()?.getCapabilities?.()?.deviceId,
          }));
        });
      }
      if (mic) {
        Promise.all([DingRTC.getMicrophones(), DingRTC.getPlaybackDevices()]).then((result) => {
          const [micList, speakerList] = result;
          const newMicList = micList.filter((item) => item.deviceId);
          const newSpeakerList = speakerList.filter((item) => item.deviceId);
          const currentMicId = mic.getMediaStreamTrack()?.getCapabilities?.()?.deviceId;
          setDeviceInfo((prev) => ({
            ...prev,
            micList: newMicList,
            speakerList: newSpeakerList,
            micId: currentMicId,
            speakerId: newSpeakerList?.[0]?.deviceId,
          }));
        });
      }
    },
    [],
  );

  const openCamera = useCallback(() => {
    return DingRTC.createCameraVideoTrack({
      deviceId: cameraId,
      dimension: cameraDimension,
      frameRate: cameraFrameRate,
    }).then((track) => {
      setLoading(false);
      print('got camera track');
      getDeviceList(cameraTrack, null);
      if (!cameraId) {
        const currentCameraId = micTrack.getMediaStreamTrack()?.getCapabilities?.()?.deviceId;
        setDeviceInfo((prev) => ({
          ...prev,
          cameraId: currentCameraId,
        }));
      }
      track.on('track-ended', () => {
        setLocalChannelInfo((prev) => ({ ...prev, cameraTrack: null }));
      });
      setLocalChannelInfo((prev) => ({ ...prev, cameraTrack: track }));
      return track;
    });
  }, [cameraId, cameraDimension, cameraFrameRate, cameraMaxBitrate]);

  const openMic = useCallback(() => {
    return DingRTC.createMicrophoneAudioTrack({ deviceId: micId }).then((track) => {
      setLoading(false);
      getDeviceList(null, micTrack);
      if (!micId) {
        const currentMicId = micTrack.getMediaStreamTrack()?.getCapabilities()?.deviceId;
        setDeviceInfo((prev) => ({
          ...prev,
          micId: currentMicId,
        }));
      }
      track.on('track-ended', () => {
        setLocalChannelInfo((prev) => ({ ...prev, micTrack: null }));
      });
      setLocalChannelInfo((prev) => ({ ...prev, micTrack: track }));
      print('got mic track');
      return track;
    });
  }, [micId]);

  const openScreen = useCallback(() => {
    if (loading) return Promise.reject();
    return DingRTC.createScreenVideoTrack({
      dimension: screenDimension,
      frameRate: screenFrameRate,
    }).then((track) => {
      setLoading(false);
      print('got screen track');
      return track[0] as LocalVideoTrack;
    });
  }, [loading, screenDimension, screenFrameRate, screenMaxBitrate]);

  const operateCamera = useCallback(() => {
    if (!cameraTrack) {
      openCamera().then((track) => {
        if (scene !== 'pre') {
          setSmallViewMap((prev) => ({ ...prev, [userId]: track }));
          publish([track]);
        }
        DingRTC.getCameras().then((list) => {
          setDeviceInfo((prev) => ({ ...prev, cameraList: list.filter((item) => item.deviceId) }));
        });
      });
    } else {
      cameraTrack.setEnabled(!cameraTrack.enabled).then(() => {
        print(`cameraTrack change to ${!cameraTrack.enabled ? 'disbaled' : 'enabled'}`);
        setLocalChannelInfo((prev) => ({ ...prev, cameraTrack }));
      });
    }
  }, [openCamera, cameraTrack, publish]);

  const cameraEnabled = useMemo(
    () => cameraTrack && cameraTrack.enabled && !cameraTrack.muted,
    [cameraTrack, cameraTrack?.enabled, cameraTrack?.muted],
  );

  const micEnabled = useMemo(
    () => micTrack && micTrack.enabled && !micTrack.muted,
    [micTrack, micTrack?.enabled, micTrack?.muted],
  );

  const operateMic = useCallback(() => {
    if (!micTrack) {
      openMic().then((track) => {
        const inPre = scene === 'pre';
        if (!inPre) publish([track]);
        DingRTC.getMicrophones().then((list) => {
          setDeviceInfo((prev) => ({ ...prev, micList: list.filter((item) => item.deviceId) }));
        });
        DingRTC.getPlaybackDevices().then((list) => {
          setDeviceInfo((prev) => ({ ...prev, speakerList: list.filter((item) => item.deviceId) }));
        });
      });
    } else {
      micTrack.setEnabled(!micTrack.enabled).then(() => {
        print(`micTrack change to ${!micTrack.enabled ? 'disbaled' : 'enabled'}`);
        setLocalChannelInfo((prev) => ({ ...prev, micTrack }));
      });
    }
  }, [openMic, micTrack, publish]);

  const operateScreen = useCallback(() => {
    if (!screenTrack) {
      openScreen().then((track) => {
        setLocalChannelInfo((prev) => ({ ...prev, screenTrack: track }));
        publish([track]).catch(() => {
          track.close();
          setLocalChannelInfo((prev) => ({ ...prev, screenTrack: null }));
        });
      });
    } else {
      screenTrack?.close();
      print(`stop share screen`);
      setLocalChannelInfo((prev) => ({ ...prev, screenTrack: null }));
    }
  }, [openScreen, screenTrack, publish]);

  return {
    openMic,
    openCamera,
    operateMic,
    micEnabled,
    cameraEnabled,
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
