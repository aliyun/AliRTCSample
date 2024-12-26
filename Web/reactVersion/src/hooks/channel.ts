import {
  LocalTrack,
  LocalVideoTrack,
  RemoteAudioTrack,
  RemoteUser,
  SubscribeParam,
  TrackMediaType,
  VideoDimension,
} from 'dingrtc';
import Toast from 'dingtalk-design-desktop/es/toast';
import { useCallback, useMemo } from 'react';
import { useRecoilState, useRecoilValue, useSetRecoilState } from 'recoil';
import {
  MainViewPrefer,
  RTCStats,
  client,
  currentUserInfo,
  localChannelInfo,
  mainViewPrefer,
  remoteChannelInfo,
  remoteUserNetworks,
} from '~/store';
import { print } from '~/utils/tools';

interface EncoderConfig {
  type: 'screenTrack' | 'cameraTrack';
  track: LocalVideoTrack;
  dimension?: VideoDimension;
  frameRate?: number;
}

export const useLocalChannel = () => {
  const [
    {
      publishedTracks,
      cameraTrack,
      micTrack,
      screenTrack,
      customAudioTrack,
      customVideoTrack,
      networkQuality,
      timeLeft,
      defaultRemoteStreamType,
    },
    setLocalChannelInfo,
  ] = useRecoilState(localChannelInfo);
  const { userId, userName } = useRecoilValue(currentUserInfo);
  const rtcClient = useRecoilValue(client);
  const publish = useCallback(
    (tracks: LocalTrack[]) => {
      return rtcClient
        ?.publish(tracks)
        .then(() => {
          setLocalChannelInfo((prev) => ({ ...prev, publishedTracks: [...rtcClient.localTracks] }));
          print(`publish ${tracks.map((item) => item.trackMediaType)} tracks`);
        })
        .catch((e) => {
          Toast.info(
            `publish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
          );
          print(
            `publish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
          );
          throw e;
        });
    },
    [rtcClient],
  );

  const allUsers = useMemo(
    () => [
      {
        userId,
        videoTrack: cameraTrack,
        userName,
        audioMuted: !micTrack?.enabled,
        hasAudio: !!micTrack,
        audioTrack: micTrack,
        hasVideo: !!cameraTrack,
        videoMuted: !cameraTrack?.enabled,
        auxiliaryMuted: !screenTrack?.enabled,
        hasAuxiliary: !!screenTrack,
        auxiliaryTrack: screenTrack,
      },
      ...rtcClient.remoteUsers,
    ],
    [rtcClient.remoteUsers, cameraTrack, screenTrack, userId, userName, micTrack],
  );

  const updateEncoder = useCallback((config: EncoderConfig) => {
    const { track, dimension, frameRate, type } = config;
    track
      .setEncoderConfiguration({
        dimension,
        frameRate,
      })
      .then(() => {
        setLocalChannelInfo((prev) => ({ ...prev, [type]: track }));
      });
  }, []);

  const unpublish = useCallback(
    (tracks: LocalTrack[]) => {
      return rtcClient
        .unpublish(tracks)
        .then(() => {
          setLocalChannelInfo((prev) => ({ ...prev, publishedTracks: [...rtcClient.localTracks] }));
          print(`unpublish ${tracks.map((item) => item.trackMediaType)} tracks`);
        })
        .catch((e) => {
          Toast.info(
            `unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
          );
          print(
            `unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
          );
          throw e;
        });
    },
    [rtcClient],
  );

  return {
    userId,
    allUsers,
    client: rtcClient,
    publish,
    screenTrack,
    unpublish,
    micTrack,
    timeLeft,
    cameraTrack,
    updateEncoder,
    networkQuality,
    publishedTracks,
    customAudioTrack,
    customVideoTrack,
    setLocalChannelInfo,
    defaultRemoteStreamType,
  };
};

export const useRemoteChannel = () => {
  const [
    { mcuAudioTrack, speakers, subscribeAllVideo, remoteUsers, groups, subscribeAudio },
    setRemoteChannelInfo,
  ] = useRecoilState(remoteChannelInfo);
  const [mainPrefer, setMainPrefer] = useRecoilState(mainViewPrefer);
  const rtcClient = useRecoilValue(client);

  const subscribe = useCallback(
    (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
      return rtcClient
        .subscribe(user.userId, mediaType, auxiliary)
        .then((track: any) => {
          print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}}`);
          setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...rtcClient.remoteUsers] }));
          return track;
        })
        .catch((e) => {
          Toast.info(
            `subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
          );
          print(
            `subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
          );
          throw e;
        });
    },
    [rtcClient],
  );

  const unsubscribe = useCallback(
    (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
      if (!auxiliary && user.videoTrack) user.videoTrack.stop();
      if (auxiliary && user.auxiliaryTrack) user.auxiliaryTrack.stop();
      return rtcClient
        .unsubscribe(user.userId, mediaType, auxiliary)
        .then(() => {
          setMainPrefer((prev) => {
            if (prev.userId === user.userId && mediaType === 'video') {
              if (
                (prev.prefer === 'auxiliary' && auxiliary) ||
                (prev.prefer === 'camera' && !auxiliary)
              ) {
                return { userId: '', prefer: 'camera' };
              }
            }
            return prev;
          });
          setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...rtcClient.remoteUsers] }));
          print(`unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
        })
        .catch((e) => {
          Toast.info(
            `unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
          );
          print(
            `unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
          );
          throw e;
        });
    },
    [rtcClient],
  );

  const subscribeAllRemoteVideo = useCallback(() => {
    const subParams: SubscribeParam[] = [];
    for (const user of remoteUsers) {
      if (user.hasVideo && !user.videoTrack) {
        subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: false });
      }
      if (user.hasAuxiliary && !user.auxiliaryTrack) {
        subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: true });
      }
    }
    if (subParams.length === 0) {
      setRemoteChannelInfo((prev) => ({
        ...prev,
        subscribeAllVideo: true,
      }));
      return Promise.resolve();
    }
    return rtcClient.batchSubscribe(subParams).then((batchSubscribeResult) => {
      let tmainPrefer: MainViewPrefer;
      for (const { error, uid: usrId, auxiliary } of batchSubscribeResult) {
        if (error) {
          Toast.info(
            `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          print(
            `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          continue;
        }
        if (!tmainPrefer) {
          tmainPrefer = { userId: usrId, prefer: auxiliary ? 'auxiliary' : 'camera' };
        }
        print(`subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'}`);
        setRemoteChannelInfo((prev) => ({
          ...prev,
          remoteUsers: [...rtcClient.remoteUsers],
        }));
      }
      if (tmainPrefer) {
        setMainPrefer(tmainPrefer);
      }
      setRemoteChannelInfo((prev) => ({
        ...prev,
        subscribeAllVideo: true,
      }));
    });
  }, [rtcClient, remoteUsers]);

  const unsubscribeAllRemoteVideo = useCallback(() => {
    const unsubParams: SubscribeParam[] = [];
    for (const user of remoteUsers) {
      if (mainPrefer.userId === user.userId) {
        setMainPrefer({ userId: '', prefer: 'camera' });
      }
      if (user.videoTrack) {
        unsubParams.push({ uid: user.userId, mediaType: 'video', auxiliary: false });
      }
      if (user.auxiliaryTrack) {
        unsubParams.push({ uid: user.userId, mediaType: 'video', auxiliary: true });
      }
    }
    if (unsubParams.length === 0) {
      setRemoteChannelInfo((prev) => ({
        ...prev,
        subscribeAllVideo: false,
      }));
      return Promise.resolve();
    }
    return rtcClient.batchUnsubscribe(unsubParams).then(() => {
      for (const { uid: usrId, auxiliary } of unsubParams) {
        print(`unsubscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'}`);
        setRemoteChannelInfo((prev) => ({
          ...prev,
          remoteUsers: [...rtcClient.remoteUsers],
        }));
      }
      setRemoteChannelInfo((prev) => ({
        ...prev,
        subscribeAllVideo: false,
      }));
    });
  }, [rtcClient, remoteUsers, mainPrefer]);

  const subscribeMCUAudio = useCallback(() => {
    const fn =
      subscribeAudio === 'mcu'
        ? () => rtcClient.subscribe('mcu', 'audio')
        : () => rtcClient.subscribeGroup(subscribeAudio);
    return fn()
      .then((track) => {
        print(`subscribe mcu audio`);
        setRemoteChannelInfo((prev) => ({ ...prev, mcuAudioTrack: track as RemoteAudioTrack }));
        (track as RemoteAudioTrack).play();
        return track;
      })
      .catch((e) => {
        Toast.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        throw e;
      });
  }, [rtcClient, subscribeAudio]);

  const unsubscribeMCUAudio = useCallback(() => {
    mcuAudioTrack?.stop();
    const fn =
      subscribeAudio === 'mcu'
        ? () => rtcClient.unsubscribe('mcu', 'audio')
        : () => rtcClient.unsubscribeGroup(subscribeAudio);
    return fn()
      .then(() => {
        print(`unsubscribe mcu audio`);
        setRemoteChannelInfo((prev) => ({ ...prev, mcuAudioTrack: null }));
      })
      .catch((e) => {
        Toast.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        throw e;
      });
  }, [mcuAudioTrack, rtcClient, subscribeAudio]);

  return {
    groups,
    speakers,
    subscribe,
    unsubscribe,
    subscribeAudio,
    mcuAudioTrack,
    subscribeAllVideo,
    subscribeMCUAudio,
    unsubscribeAllRemoteVideo,
    subscribeAllRemoteVideo,
    unsubscribeMCUAudio,
    setRemoteChannelInfo,
  };
};

export const useNetworkStats = () => {
  const rtcClient = useRecoilValue(client);
  const setRemoteUserNetworks = useSetRecoilState(remoteUserNetworks);

  const [{ rtcStats, micTrack }, setLocalChannelInfo] = useRecoilState(localChannelInfo);
  const { mcuAudioTrack } = useRecoilValue(remoteChannelInfo);
  const getRtcStats = useCallback(() => {
    Promise.all([
      rtcClient.getLocalAudioStats(),
      rtcClient.getLocalVideoStats(),
      rtcClient.getRemoteAudioStats(),
      rtcClient.getRemoteVideoStats(),
    ]).then(([localAudioStats, localVideoStats, remoteAudioStats, remoteVideoStats]) => {
      setLocalChannelInfo((prev) => {
        const { camera, auxiliary } = localVideoStats;
        let maxCameraRemoteResolution: { width: number; height: number };
        let maxCameraFps = 0;
        let maxScreenRemoteResolution: { width: number; height: number };
        let maxScreenFps = 0;
        let maxLoss = 0;
        let recvCameraBitrate = 0;
        let recvScreenBitrate = 0;
        let maxRtt = Math.max(localAudioStats.rtt ?? 0, camera?.rtt ?? 0, auxiliary?.rtt ?? 0, 0);
        // if (localAudioStats.sendPacketsLost) {
        //   maxLoss = Math.max(localAudioStats.sendPacketsLost / localAudioStats.sendPackets, maxLoss);
        // }
        // if (camera?.sendPacketsLost) {
        //   maxLoss = Math.max(
        //     camera.sendPacketsLost / camera.sendPackets,
        //     maxLoss,
        //   );
        // }
        // if (auxiliary?.sendPacketsLost) {
        //   maxLoss =  Math.max(auxiliary.sendPacketsLost / auxiliary.sendPackets, maxLoss);
        // }
        const sendBitrate =
          (localAudioStats?.sendBitrate ?? 0) +
          (camera?.sendBitrate ?? 0) +
          (auxiliary?.sendBitrate ?? 0);
        let recvBtrate = remoteAudioStats?.receiveBitrate || 0;
        for (const remoteVideo of Object.values(remoteVideoStats)) {
          const { camera: rCamera, auxiliary: rAuxiliary } = remoteVideo;
          if (rAuxiliary) {
            const {
              receiveFrameRate,
              receiveResolution,
              receiveBitrate,
              rtt,
              receivePackets,
              receivePacketsLost,
            } = rAuxiliary;
            maxScreenFps = Math.max(maxScreenFps, receiveFrameRate || 0);
            maxRtt = Math.max(maxRtt, rtt ?? 0);
            if (receivePacketsLost) {
              maxLoss = Math.max(maxLoss, receivePacketsLost / receivePackets);
            }
            recvBtrate += receiveBitrate || 0;
            recvScreenBitrate += receiveBitrate ?? 0;
            if (!maxScreenRemoteResolution) {
              maxScreenRemoteResolution = receiveResolution;
            } else if (
              receiveResolution.height * receiveResolution.width >
              maxScreenRemoteResolution.width * maxScreenRemoteResolution.height
            ) {
              maxScreenRemoteResolution = receiveResolution;
            }
          }
          if (rCamera) {
            const {
              receiveFrameRate,
              receiveResolution,
              receiveBitrate,
              rtt,
              receivePackets,
              receivePacketsLost,
            } = rCamera;
            recvBtrate += receiveBitrate || 0;
            maxRtt = Math.max(maxRtt, rtt ?? 0);
            recvCameraBitrate += receiveBitrate ?? 0;
            if (receivePacketsLost) {
              maxLoss = Math.max(maxLoss, receivePacketsLost / receivePackets);
            }
            maxCameraFps = Math.max(maxCameraFps, receiveFrameRate || 0);
            if (!maxCameraRemoteResolution) {
              maxCameraRemoteResolution = receiveResolution;
            } else if (
              receiveResolution.height * receiveResolution.width >
              maxCameraRemoteResolution.width * maxCameraRemoteResolution.height
            ) {
              maxCameraRemoteResolution = receiveResolution;
            }
          }
        }
        const newRtcStats: RTCStats = {
          uplinkProfile: (localVideoStats.uplinkProfile || []).join(';'),
          downlinkProfile: (localVideoStats.downlinkProfile || []).join(';'),
          localCameraFPS: camera?.sendFrameRate,
          localCameraBitrate: camera?.sendBitrate,
          localCameraResolution: camera?.sendResolution,
          localScreenFPS: auxiliary?.sendFrameRate,
          localScreenBitrate: auxiliary?.sendBitrate,
          localBitrate: sendBitrate,
          remoteBitrate: recvBtrate,
          localScreenResolution: auxiliary?.sendResolution,
          localAudioBitrate: localAudioStats?.sendBitrate,
          remoteCameraFPS: maxCameraFps,
          remoteCameraBitrate: recvCameraBitrate,
          encodeCameraLayers: camera?.encodeLayers,
          encodeScreenLayers: auxiliary?.encodeLayers,
          sendCameraLayers: camera?.sendLayers,
          sendScreenLayers: auxiliary?.sendLayers,
          remoteScreenBitrate: recvScreenBitrate,
          remoteCameraResolution: maxCameraRemoteResolution,
          remoteScreenFPS: maxScreenFps,
          remoteScreenResolution: maxScreenRemoteResolution,
          remoteAudioBitrate: remoteAudioStats?.receiveBitrate,
          // loss: Math.round(maxLoss * 100),
          localAudioLevel: micTrack ? Math.round(micTrack.getVolumeLevel() * 100) : 0,
          remoteAudioLevel: mcuAudioTrack ? Math.round(mcuAudioTrack.getVolumeLevel() * 100) : 0,
          rtt: maxRtt,
        };
        return {
          ...prev,
          rtcStats: {
            ...prev.rtcStats,
            ...newRtcStats,
          },
        };
      });
    });
  }, [mcuAudioTrack, micTrack]);

  const getRemoteUserNetworkStats = useCallback(() => {
    const result = rtcClient.getRemoteNetworkQuality?.();
    if (result) {
      const newStats: any = {};
      for (const uid in result) {
        if (result[uid]) {
          newStats[uid] = Math.max(
            result[uid].uplinkNetworkQuality,
            result[uid].downlinkNetworkQuality,
          );
        }
      }
      setRemoteUserNetworks(newStats);
    }
  }, []);

  return {
    rtcStats,
    getRtcStats,
    getRemoteUserNetworkStats,
  };
};
