import {
  LocalTrack,
  LocalVideoTrack,
  NetworkQuality,
  RemoteAudioTrack,
  RemoteUser,
  SubscribeParam,
  TrackMediaType,
  VideoDimension,
} from 'dingrtc-electron-sdk';
import { message } from 'ant-design-vue';
import { toRaw } from 'vue';
import { useClient, useChannelInfo, useGlobalFlag, useCurrentUserInfo, useDeviceInfo } from '~/store';
import { print } from '~/utils/tools';
import { UserVolumeLevelInfo } from 'dingrtc-electron-sdk';

interface EncoderConfig {
  type: 'screenTrack' | 'cameraTrack';
  track: LocalVideoTrack;
  dimension?: VideoDimension;
  frameRate?: number;
}

export const useChannel = () => {
  const channelInfo = useChannelInfo();
  const currentUserInfo = useCurrentUserInfo();
  const client = useClient();
  const publish = (tracks: LocalTrack[]) => {
    const tempTracks = tracks.map(item => toRaw(item));
    return toRaw(client).publish(tempTracks)
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
    return toRaw(client)
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
    return toRaw(client)
      .subscribe(user.userId, mediaType, auxiliary)
      .then((track: any) => {
        print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
        channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
        channelInfo.updateTrackStats(user.userId);
        channelInfo.$patch((state) => {
          if (mediaType === 'video') {
            if (state.mainViewUserId === currentUserInfo.userId) {
              state.mainViewUserId = user.userId;
              state.mainViewPreferType = auxiliary ? 'auxiliary' : 'camera';
            } else if (state.mainViewUserId === user.userId) {
              state.forceUpdateMainViewTrack = true;
            }
          }
        });
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
    return toRaw(client)
      .unsubscribe(user.userId, mediaType, auxiliary)
      .then(() => {
        channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
        channelInfo.updateTrackStats(user.userId);
        channelInfo.$patch((state) => {
          if (state.mainViewUserId === user.userId && mediaType === 'video') {
            if (
              (state.mainViewPreferType === 'auxiliary' && auxiliary) ||
              (state.mainViewPreferType === 'camera' && !auxiliary)
            ) {
              for (const u of channelInfo.remoteUsers) {
                if (u.userId !== user.userId && (u.auxiliaryTrack || u.videoTrack)) {
                  state.mainViewUserId = u.userId;
                  state.mainViewPreferType = u.auxiliaryTrack ? 'auxiliary' : 'camera';
                  break;
                }
              }
              if (state.mainViewUserId === user.userId) { // no other video is subscribed
                state.mainViewUserId = currentUserInfo.userId;
                state.mainViewPreferType = 'camera'
              }
            }
          }
        });
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
    return toRaw(client).batchSubscribe(subParams).then((batchSubscribeResult) => {
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
        remoteUsers: [...client.remoteUsers],
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
    return toRaw(client).batchUnsubscribe(unsubParams).then(() => {
      for (const { uid, auxiliary } of unsubParams) {
        print(`unsubscribe user ${uid} ${auxiliary ? 'screenShare' : 'camera'}`);
        channelInfo.$patch({
          remoteUsers: [...client.remoteUsers],
        });
        channelInfo.updateTrackStats(uid);
      }
      channelInfo.$patch({ subscribeAllVideo: false });
    });
  };

  const subscribeMCUAudio = () => {
    const fn = () => toRaw(client).subscribe('mcu', 'audio')
    return fn()
      .then((track) => {
        print(`subscribe mcu audio`);
        channelInfo.$patch({ mcuAudioTrack: track as RemoteAudioTrack });
        return track;
      })
      .catch((e) => {
        message.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
        throw e;
      });
  };

  const unsubscribeMCUAudio = () => {
    const fn = () => toRaw(client).unsubscribe('mcu', 'audio')
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
    getRemoteUserNetworkStats,
  };
};

export const useInChannelHooks = () => {
  const channelInfo = useChannelInfo();
  const currentUserInfo = useCurrentUserInfo();
  const deviceInfo = useDeviceInfo();
  const client = useClient();
  const globalFlag = useGlobalFlag();
  const { subscribe, checkMainview } = useChannel();

  const bindEvents = () => {
    client.on('user-unpublished', (user, mediaType, auxiliary) => {
      print(
        `user ${user.userId} unpublished ${mediaType === 'audio' ? 'audio' : auxiliary ? 'screenShare' : mediaType}}`,
      );
      channelInfo.updateTrackStats(user.userId);
      channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
    });
    client.on('user-published', (user, mediaType, auxiliary) => {
      print(
        `user ${user.userId} published ${mediaType === 'audio' ? 'audio' : auxiliary ? 'screenShare' : mediaType}}`,
      );
      channelInfo.updateTrackStats(user.userId);
      channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
      if (mediaType !== 'video') {
        return;
      }
      if (channelInfo.subscribeAllVideo) {
        subscribe(user, 'video', auxiliary);
      }
    });
    client.on('user-joined', (user) => {
      print(`user ${user.userId} joined`, user);
      channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
      channelInfo.updateTrackStats(user.userId);
    });
    client.on('stream-type-changed', (uid, streamType) => {
      print(`user ${uid} streamType changeTo ${streamType}`);
    });
  
    client.on('connection-state-change', (current, _, reason) => {
      print(`connection-state-change ${current} ${reason || ''}`);
      if (current === 'disconnected') {
        if (reason !== 'leave') {
          message.info(reason);
        }
        clearRoom();
      }
    });
    client.on('network-quality', (uplink, downlink) => {
      print('network-quality', uplink, downlink)
      //channelInfo.$patch({ networkQuality: Math.min(uplink, downlink) as NetworkQuality });
    });
    client.on('user-network-quality', (uid, uplink, downlink) => {
      print('user-network-quality', uid, uplink, downlink)
      let quality = Math.max(uplink, downlink);
      channelInfo.updateUserNetworkQuality(uid, quality);
    });
    client.on('volume-indicator', (speakers: UserVolumeLevelInfo[]) => {
      const speakingUids = speakers.filter(item => {
          return item.speechState && (item.uid !== currentUserInfo.userId || deviceInfo.micEnable);
        }).map(item => item.uid);
      if (speakingUids.length) {
        print(`${speakingUids.join()} is speaking`);
      }
      channelInfo.$patch({ speakers: speakingUids });
    });
    client.on('user-info-updated', (uid, msg) => {
      print(`user ${uid}: ${msg}`);
      channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
      channelInfo.updateTrackStats(uid);
    });
    client.on('user-left', (user) => {
      print(`user ${user.userId} left`);
      checkMainview(user);
      channelInfo.updateTrackStats(user.userId);
      channelInfo.$patch({ remoteUsers: [...client.remoteUsers] });
    });
    client.on('quality-report', report => {
      channelInfo.$patch({ networkQuality: report.signalLevel as NetworkQuality });
      channelInfo.$patch({ rtcStats: report })
    })
  }

  const clearRoom = () => {
    if (document.fullscreenElement) {
      document.exitFullscreen();
    }
    if (channelInfo.cameraTrack) {
      toRaw(channelInfo.cameraTrack).close();
    }
    if (channelInfo.screenTrack) {
      toRaw(channelInfo.screenTrack).close();
    }
    channelInfo.remoteUsers.forEach((user) => {
      if (user.auxiliaryTrack) {
        toRaw(user.auxiliaryTrack).stop();
      }
      if (user.videoTrack) {
        toRaw(user.videoTrack).stop();
      }
    });
    client.removeAllListeners();
    channelInfo.$reset();
    globalFlag.$reset();
  };

  return {
    clearRoom,
    bindEvents,
  }
}