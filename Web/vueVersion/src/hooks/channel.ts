import {
  LocalTrack,
  LocalVideoTrack,
  RemoteAudioTrack,
  RemoteUser,
  SubscribeParam,
  TrackMediaType,
  VideoDimension,
} from 'dingrtc';
import { AnnotationSourceType } from '@dingrtc/whiteboard';
import { message } from 'ant-design-vue';
import { markRaw, toRaw } from 'vue';
import {
  RTCStats,
  useClient,
  useChannelInfo,
  useAsrInfo,
  useRTMInfo,
  useCurrentUserInfo,
} from '~/store';
import { logger } from '~/utils/tools';

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
    const tempTracks = tracks.map((item) => toRaw(item));
    return client
      .publish(tempTracks)
      .then(() => {
        channelInfo.updatePublishedTracks(
          tempTracks.map((item) => item.getTrackId()),
          'add',
        );
        logger.info(`publish ${tracks.map((item) => item.trackMediaType)} tracks`);
      })
      .catch((e) => {
        message.info(
          `publish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
        );
        logger.info(
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
    const tempTracks = tracks.map((item) => toRaw(item));
    return client
      .unpublish(tempTracks)
      .then(() => {
        channelInfo.updatePublishedTracks(
          tempTracks.map((item) => item.getTrackId()),
          'remove',
        );
        logger.info(`unpublish ${tracks.map((item) => item.trackMediaType)} tracks`);
      })
      .catch((e) => {
        message.info(
          `unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
        );
        logger.info(
          `unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`,
        );
        throw e;
      });
  };

  const subscribe = (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
    return client
      .subscribe(user.userId, mediaType, auxiliary)
      .then((track: any) => {
        logger.info(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
        channelInfo.$patch({ remoteUsers: client.remoteUsers });
        channelInfo.updateTrackStats(user.userId);
        return track;
      })
      .catch((e) => {
        message.info(
          `subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
        );
        logger.info(
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
        logger.info(`unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
      })
      .catch((e) => {
        message.info(
          `unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`,
        );
        logger.info(
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
          logger.info(
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
        logger.info(`subscribe user ${uid} ${auxiliary ? 'screenShare' : 'camera'}`);
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
        logger.info(`unsubscribe user ${uid} ${auxiliary ? 'screenShare' : 'camera'}`);
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
        logger.info(`subscribe mcu audio`);
        channelInfo.$patch({ mcuAudioTrack: track as RemoteAudioTrack });
        (track as RemoteAudioTrack).play();
        return track;
      })
      .catch((e) => {
        message.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        logger.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
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
        logger.info(`unsubscribe mcu audio`);
        channelInfo.$patch({ mcuAudioTrack: null });
      })
      .catch((e) => {
        message.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        logger.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
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
  const currentUserInfo = useCurrentUserInfo();
  const getRtcStats = () => {
    Promise.all([
      client.getLocalAudioStats(),
      client.getLocalVideoStats(),
      client.getRemoteAudioStats(),
      client.getRemoteVideoStats(),
    ]).then(([localAudioStats, localVideoStats, remoteAudioStats, remoteVideoStats]) => {
      channelInfo.$patch((prev) => {
        const resolutionMap = new Map();
        const { camera, auxiliary } = localVideoStats;
        if (camera) {
          resolutionMap.set(
            `${currentUserInfo.userId}#camera`,
            `${camera.sendResolution.width}*${camera.sendResolution.height}`,
          );
        }
        if (auxiliary) {
          resolutionMap.set(
            `${currentUserInfo.userId}#auxiliary`,
            `${auxiliary.sendResolution.width}*${auxiliary.sendResolution.height}`,
          );
        }
        let maxCameraRemoteResolution: { width: number; height: number };
        let maxCameraFps = 0;
        let maxScreenRemoteResolution: { width: number; height: number };
        let maxScreenFps = 0;
        let maxLoss = 0;
        let recvCameraBitrate = 0;
        let recvScreenBitrate = 0;
        const bwe = camera?.outgingBitrate || 0;
        let maxRtt = Math.max(localAudioStats.rtt ?? 0, camera?.rtt ?? 0, auxiliary?.rtt ?? 0, 0);
        const sendBitrate =
          (localAudioStats?.sendBitrate ?? 0) +
          (camera?.sendBitrate ?? 0) +
          (auxiliary?.sendBitrate ?? 0);
        let recvBtrate = remoteAudioStats?.receiveBitrate || 0;
        for (const [uid, remoteVideo] of Object.entries(remoteVideoStats)) {
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
            resolutionMap.set(
              `${uid}#auxiliary`,
              `${receiveResolution.width}*${receiveResolution.height}`,
            );
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
            resolutionMap.set(
              `${uid}#camera`,
              `${receiveResolution.width}*${receiveResolution.height}`,
            );
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
          resolutionMap,
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
          bwe,
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

  const openWhiteboard = (id: string) => {
    if (channelInfo.annotation || channelInfo.whiteboard) return;
    const t = channelInfo.whiteboardManager.getWhiteboard(id);
    // @ts-ignore
    window.t = t;
    const whiteboard = markRaw(t);
    channelInfo.whiteboard = whiteboard;
    preMode = channelInfo.mode;
    channelInfo.$patch({
      mainViewUserId: '',
      isWhiteboardOpen: true,
      mode: 'standard',
      mainViewPreferType: 'auxiliary',
    });
  };

  const openAnnotation = (annotationId: string, sourceType: AnnotationSourceType) => {
    logger.info(annotationId, sourceType);
    if (channelInfo.annotationId === `${annotationId}#${sourceType}`) return;
    preMode = channelInfo.mode;
    const annotation = channelInfo.whiteboardManager.getAnnotation(annotationId, sourceType);
    const [_, trackUserId] = annotationId.split('_');
    channelInfo.annotation = markRaw(annotation);
    if (channelInfo.isWhiteboardOpen) channelInfo.whiteboard.close();
    channelInfo.$patch({
      isWhiteboardOpen: false,
      mode: 'standard',
      annotationId: `${annotationId}#${sourceType}`,
      mainViewUserId: trackUserId,
      mainViewPreferType: sourceType === 'share' ? 'auxiliary' : 'camera',
    });
  };

  const closeAnnotation = (id: string, sourceType: AnnotationSourceType) => {
    const annotation = channelInfo.whiteboardManager.getAnnotation(id, sourceType);
    if (annotation === channelInfo.annotation) {
      channelInfo.annotation?.close();
      channelInfo.$patch({
        mode: preMode,
        annotation: null,
        annotationId: '',
      });
    }
  };

  const closeWhiteboard = (id: string) => {
    if (channelInfo.whiteboard?.sessionId === id) {
      channelInfo.isWhiteboardOpen = false;
      channelInfo.whiteboard.close();
      channelInfo.whiteboard = null;
      channelInfo.mode = preMode;
    }
  };

  return {
    openAnnotation,
    openWhiteboard,
    closeAnnotation,
    closeWhiteboard,
  };
};

export const useAsrHooks = () => {
  const asrInfo = useAsrInfo();

  const changeTranslateLanguages = (lang: string) => {
    if (asrInfo.transLang !== lang) {
      asrInfo.transLang = lang;
      const newLangs = [lang];
      if (asrInfo.dualLang) {
        newLangs.push('source');
      }
      asrInfo.asr.setCurrentTranslateLanguages(newLangs);
    }
  };

  const changeDualLang = (value: boolean) => {
    const newLangs = [asrInfo.transLang];
    asrInfo.dualLang = value;
    if (value) {
      newLangs.push('source');
    }
    asrInfo.asr.setCurrentTranslateLanguages(newLangs);
  };

  return {
    changeTranslateLanguages,
    changeDualLang,
  };
};

export const useRTMInfoHooks = () => {
  const rtmInfo = useRTMInfo();

  const initSessions = () => {
    rtmInfo.sessions = rtmInfo.rtm.sessions.map((item) => ({
      sessionId: item.id,
      members: item.users,
      messages: [],
    }));
  };

  const joinSession = (sessionId: string) => {
    rtmInfo.rtm
      .joinSession(sessionId)
      .then(() => {
        const session = rtmInfo.sessions.find((item) => item.sessionId === sessionId);
        if (session) {
          const rtmSession = rtmInfo.rtm.sessions.find((item) => item.id === sessionId);
          const existed = session.members.map((item) => item.userId);
          rtmSession.users.forEach((item) => {
            if (!existed.includes(item.userId)) {
              session.members.push(item);
            }
          });
        }
      })
      .catch((e) => message.error(`joinSession failed: ${JSON.stringify(e)}`));
  };

  const leaveSession = (sessionId: string) => {
    rtmInfo.rtm
      .leaveSession(sessionId)
      .then(() => {
        const session = rtmInfo.sessions.find((item) => item.sessionId === sessionId);
        session?.members.splice(0);
        session?.messages.splice(0);
      })
      .catch((e) => message.error(`leaveSession failed: ${JSON.stringify(e)}`));
  };

  const closeSession = (sessionId: string) => {
    rtmInfo.rtm
      .closeSession(sessionId)
      .then(() => {
        const session = rtmInfo.sessions.find((item) => item.sessionId === sessionId);
        session?.members.splice(0);
        session?.messages.splice(0);
      })
      .catch((e) => message.error(`closeSession failed: ${JSON.stringify(e)}`));
  };

  return {
    leaveSession,
    joinSession,
    closeSession,
    initSessions,
  };
};
