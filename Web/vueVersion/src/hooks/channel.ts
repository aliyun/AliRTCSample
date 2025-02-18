import {
  LocalTrack,
  LocalVideoTrack,
  RemoteAudioTrack,
  RemoteUser,
  SubscribeParam,
  TrackMediaType,
  VideoDimension,
} from 'dingrtc';
import { message } from 'ant-design-vue';
import { toRaw } from 'vue';
import { RTCStats, useClient, useChannelInfo } from '~/store';
import { print } from '~/utils/tools';

interface EncoderConfig {
  type: 'screenTrack' | 'cameraTrack';
  track: LocalVideoTrack;
  dimension?: VideoDimension;
  frameRate?: number;
}

export const useChannel = () => {
  const channelInfo = useChannelInfo();
  const client = useClient();
  const publish = (tracks: LocalTrack[]) => {
    const tempTracks = tracks.map(item => toRaw(item));
    return client
      .publish(tempTracks)
      .then(() => {
        channelInfo.updatePublishedTracks(tempTracks.map(item => item.getTrackId()), 'add')
        print(`publish ${tracks.map((item) => item.trackMediaType)} tracks`);
      })
      .catch((e) => {
        message.info(
          `publish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
        );
        print(
          `publish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const updateEncoder = (config: EncoderConfig) => {
    const { track, dimension, frameRate, type } = config;
    track
      .setEncoderConfiguration({
        dimension,
        frameRate,
      })
      .then(() => {
        channelInfo.$patch({ [type]: track });
      });
  };

  const unpublish = (tracks: LocalTrack[]) => {
    const tempTracks = tracks.map(item => toRaw(item));
    return client
      .unpublish(tempTracks)
      .then(() => {
        channelInfo.updatePublishedTracks(tempTracks.map(item => item.getTrackId()), 'remove')
        print(`unpublish ${tracks.map((item) => item.trackMediaType)} tracks`);
      })
      .catch((e) => {
        message.info(
          `unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
        );
        print(
          `unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const subscribe = (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
    return client
      .subscribe(user.userId, mediaType, auxiliary)
      .then((track: any) => {
        print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
        channelInfo.$patch({ remoteUsers: client.remoteUsers });
        channelInfo.updateTrackStats(user.userId);
        return track;
      })
      .catch((e) => {
        message.info(
          `subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
        );
        print(
          `subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const unsubscribe = (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
    if (!auxiliary && user.videoTrack) user.videoTrack.stop();
    if (auxiliary && user.auxiliaryTrack) user.auxiliaryTrack.stop();
    return client
      .unsubscribe(user.userId, mediaType, auxiliary)
      .then(() => {
        channelInfo.$patch((state) => {
          if (state.mainViewUserId === user.userId && mediaType === 'video') {
            if (
              (state.mainViewPreferType === 'auxiliary' && auxiliary) ||
              (state.mainViewPreferType === 'camera' && !auxiliary)
            ) {
              state.mainViewUserId = '';
              state.mainViewPreferType = 'camera';
            }
          }
        });
        channelInfo.updateTrackStats(user.userId);
        channelInfo.$patch({ remoteUsers: client.remoteUsers });
        print(`unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
      })
      .catch((e) => {
        message.info(
          `unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
        );
        print(
          `unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const subscribeAllRemoteVideo = () => {
    const subParams: SubscribeParam[] = [];
    for (const user of channelInfo.remoteUsers) {
      if (user.hasVideo && !user.videoTrack) {
        subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: false });
      }
      if (user.hasAuxiliary && !user.auxiliaryTrack) {
        subParams.push({ uid: user.userId, mediaType: 'video', auxiliary: true });
      }
    }
    if (subParams.length === 0) {
      channelInfo.$patch({
        subscribeAllVideo: true,
      });
      return Promise.resolve();
    }
    return client.batchSubscribe(subParams).then((batchSubscribeResult) => {
      for (const { error, uid, auxiliary } of batchSubscribeResult) {
        if (error) {
          message.info(
            `subscribe user ${uid} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          print(
            `subscribe user ${uid} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          continue;
        }
        if (!channelInfo.mainViewUserId) {
          channelInfo.$patch({
            mainViewPreferType: auxiliary ? 'auxiliary' : 'camera',
            mainViewUserId: uid,
          });
        }
        print(`subscribe user ${uid} ${auxiliary ? 'screenShare' : 'camera'}`);
        channelInfo.updateTrackStats(uid);
      }
      channelInfo.$patch({
        subscribeAllVideo: true,
        remoteUsers: client.remoteUsers,
      });
    });
  };

  const checkMainview = (user) => {
    if (channelInfo.mainViewUserId === user.userId) {
      channelInfo.mainViewTrack?.stop();
      channelInfo.$patch({
        mainViewPreferType: 'auxiliary',
        mainViewUserId: '',
      });
    }
  };

  const unsubscribeAllRemoteVideo = () => {
    const unsubParams: SubscribeParam[] = [];
    for (const user of channelInfo.remoteUsers) {
      checkMainview(user);
      if (user.videoTrack) {
        unsubParams.push({ uid: user.userId, mediaType: 'video', auxiliary: false });
      }
      if (user.auxiliaryTrack) {
        unsubParams.push({ uid: user.userId, mediaType: 'video', auxiliary: true });
      }
    }
    if (unsubParams.length === 0) {
      channelInfo.$patch({
        subscribeAllVideo: false,
      });
      return Promise.resolve();
    }
    return client.batchUnsubscribe(unsubParams).then(() => {
      for (const { uid, auxiliary } of unsubParams) {
        print(`unsubscribe user ${uid} ${auxiliary ? 'screenShare' : 'camera'}`);
        channelInfo.$patch({
          remoteUsers: client.remoteUsers,
        });
        channelInfo.updateTrackStats(uid);
      }
      channelInfo.$patch({ subscribeAllVideo: false });
    });
  };

  const subscribeMCUAudio = () => {
    const fn =
      channelInfo.subscribeAudio === 'mcu'
        ? () => client.subscribe('mcu', 'audio')
        : () => client.subscribeGroup(channelInfo.subscribeAudio);
    return fn()
      .then((track) => {
        print(`subscribe mcu audio`);
        channelInfo.$patch({ mcuAudioTrack: track as RemoteAudioTrack });
        (track as RemoteAudioTrack).play();
        return track;
      })
      .catch((e) => {
        message.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        throw e;
      });
  };

  const unsubscribeMCUAudio = () => {
    channelInfo.mcuAudioTrack?.stop();
    const fn =
      channelInfo.subscribeAudio === 'mcu'
        ? () => client.unsubscribe('mcu', 'audio')
        : () => client.unsubscribeGroup(channelInfo.subscribeAudio);
    return fn()
      .then(() => {
        print(`unsubscribe mcu audio`);
        channelInfo.$patch({ mcuAudioTrack: null });
      })
      .catch((e) => {
        message.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        throw e;
      });
  };

  return {
    publish,
    unpublish,
    updateEncoder,
    subscribe,
    unsubscribe,
    checkMainview,
    subscribeMCUAudio,
    unsubscribeAllRemoteVideo,
    subscribeAllRemoteVideo,
    unsubscribeMCUAudio,
  };
};

export const useNetworkStats = () => {
  const client = useClient();
  const channelInfo = useChannelInfo();
  const getRtcStats = () => {
    Promise.all([
      client.getLocalAudioStats(),
      client.getLocalVideoStats(),
      client.getRemoteAudioStats(),
      client.getRemoteVideoStats(),
    ]).then(([localAudioStats, localVideoStats, remoteAudioStats, remoteVideoStats]) => {
      channelInfo.$patch((prev) => {
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
          sendCameraLayers: camera?.sendLayers,
          sendScreenLayers: auxiliary?.sendLayers,
          remoteScreenBitrate: recvScreenBitrate,
          remoteCameraResolution: maxCameraRemoteResolution,
          remoteScreenFPS: maxScreenFps,
          remoteScreenResolution: maxScreenRemoteResolution,
          remoteAudioBitrate: remoteAudioStats?.receiveBitrate,
          // loss: Math.round(maxLoss * 100),
          localAudioLevel: channelInfo.micTrack
            ? Math.round(channelInfo.micTrack.getVolumeLevel() * 100)
            : 0,
          remoteAudioLevel: channelInfo.mcuAudioTrack
            ? Math.round(channelInfo.mcuAudioTrack.getVolumeLevel() * 100)
            : 0,
          rtt: maxRtt,
        };
        // channelInfo.$state = {
        //   ...prev,
        //   rtcStats: {
        //              ...prev.rtcStats,
        //   ...newRtcStats,
        //   }
        // }
        prev.rtcStats = newRtcStats;
      });
    });
  };

  const getRemoteUserNetworkStats = () => {
    const result = client.getRemoteNetworkQuality?.();
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
      channelInfo.$patch({ remoteUserNetworkQualitys: newStats });
    }
  };

  return {
    getRtcStats,
    getRemoteUserNetworkStats,
  };
};

export const useWhiteboardHooks = () => {
  const channelInfo = useChannelInfo();
  let preMode = channelInfo.mode;


  const openWhiteboard = () => {
    preMode = channelInfo.mode;
    channelInfo.$patch({
      mainViewUserId: '',
      isWhiteboardOpen: true,
      mode: 'standard',
      mainViewPreferType: 'auxiliary',
    });
  };
  
  const closeWhiteboard = () => {
    channelInfo.isWhiteboardOpen = false;
    channelInfo.mode = preMode;
  };

  return {
    openWhiteboard,
    closeWhiteboard,
  }
}