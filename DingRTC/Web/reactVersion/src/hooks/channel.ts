import {
  LocalTrack,
  LocalVideoTrack,
  RemoteAudioTrack,
  RemoteUser,
  RemoteVideoTrack,
  SubscribeParam,
  TrackMediaType,
  VideoDimension,
} from 'dingrtc';
import Toast from 'dingtalk-design-desktop/es/toast';
import { useCallback } from 'react';
import { useRecoilState, useRecoilValue, useSetRecoilState } from 'recoil';
import {
  client,
  currentUserInfo,
  localChannelInfo,
  mainView,
  remoteChannelInfo,
  smallViewTrackMap,
} from '~/store';
import { print } from '~/utils/tools';

interface EncoderConfig {
  type: 'screenTrack' | 'cameraTrack';
  track: LocalVideoTrack;
  dimension?: VideoDimension;
  frameRate?: number;
}

export const useMainView = () => {
  const [mainViewTrack, setMainView] = useRecoilState(mainView);
  const { userId } = useRecoilValue(currentUserInfo);
  const [viewMap, setSmallViewMap] = useRecoilState(smallViewTrackMap);

  const updateMainView = useCallback(
    (user: RemoteUser, newTrack: RemoteVideoTrack | LocalVideoTrack) => {
      const mainTrackUid = mainViewTrack
        ? (mainViewTrack as RemoteVideoTrack)?.getUserId?.() || userId
        : '';
      const newTrackUid = (newTrack as RemoteVideoTrack)?.getUserId?.() || userId;
      if (newTrack === mainViewTrack) {
        mainViewTrack.stop();
        viewMap[mainTrackUid] = mainViewTrack;
        setMainView(null);
      } else {
        mainViewTrack?.stop();
        newTrack?.stop();
        if (newTrackUid !== mainTrackUid) {
          delete viewMap[newTrackUid];
          if (mainViewTrack) {
            const oldSmallViewTrack = viewMap[mainTrackUid];
            oldSmallViewTrack?.stop();
            viewMap[mainTrackUid] = mainViewTrack;
          }
          if (newTrack === user.videoTrack && user.auxiliaryTrack) {
            viewMap[newTrackUid] = user.auxiliaryTrack;
          } else if (newTrack === user.auxiliaryTrack && user.videoTrack) {
            viewMap[newTrackUid] = user.videoTrack;
          }
        }
        if (newTrackUid === mainTrackUid) {
          if (newTrack === user.videoTrack && mainViewTrack === user.auxiliaryTrack) {
            viewMap[newTrackUid] = user.auxiliaryTrack;
          } else if (newTrack === user.auxiliaryTrack && mainViewTrack === user.videoTrack) {
            viewMap[newTrackUid] = user.videoTrack;
          }
        }
        setMainView(newTrack);
      }
      setSmallViewMap({ ...viewMap });
    },
    [mainViewTrack, userId, viewMap],
  );

  return {
    updateMainView,
  };
};

export const useLocalChannel = () => {
  const [
    {
      publishedTracks,
      cameraTrack,
      micTrack,
      screenTrack,
      customAudioTrack,
      customVideoTrack,
    },
    setLocalChannelInfo,
  ] = useRecoilState(localChannelInfo);
  const IClient = useRecoilValue(client);
  const publish = useCallback((tracks: LocalTrack[]) => {
    return IClient?.publish(tracks).then(() => {
      setLocalChannelInfo((prev) => ({ ...prev, publishedTracks: [...IClient.localTracks] }));
      print(`publish ${tracks.map((item) => item.trackMediaType)} tracks`);
    }).catch(e => {
      Toast.info(`publish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`);
      print(`publish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`);
      throw e;
    });
  }, [IClient]);

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

  const unpublish = useCallback((tracks: LocalTrack[]) => {
    return IClient.unpublish(tracks).then(() => {
      setLocalChannelInfo((prev) => ({ ...prev, publishedTracks: [...IClient.localTracks] }));
      print(`unpublish ${tracks.map((item) => item.trackMediaType)} tracks`);
    }).catch(e => {
      Toast.info(`unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`);
      print(`unpublish ${tracks.map((item) => item.trackMediaType)} tracks failed: ${JSON.stringify(e)}`);
      throw e;
    });
  }, [IClient]);

  return {
    client: IClient,
    publish,
    screenTrack,
    unpublish,
    micTrack,
    cameraTrack,
    updateEncoder,
    publishedTracks,
    customAudioTrack,
    customVideoTrack,
    setLocalChannelInfo
  };
};

export const useRemoteChannel = () => {
  const [{ mcuAudioTrack, speakers, subscribeAllVideo, remoteUsers }, setRemoteChannelInfo] = useRecoilState(remoteChannelInfo);
  const [mainViewTrack, setMainView] = useRecoilState(mainView);
  const IClient = useRecoilValue(client);

  const setSmallViewTrackMap = useSetRecoilState(smallViewTrackMap);

  const subscribe = useCallback(
    (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
      return IClient.subscribe(user.userId, mediaType, auxiliary).then((track: any) => {
        print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}}`);
        setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
        setSmallViewTrackMap((prev) => {
          if (auxiliary && prev[user.userId]) {
            return prev;
          }
          return ({ ...prev, [user.userId]: track })
        });
        return track;
      }).catch(e => {
        Toast.info(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`);
        print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`);
        throw e;
      });
    },
    [IClient],
  );

  const unsubscribe = useCallback(
    (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
      if (!auxiliary && user.videoTrack) user.videoTrack.stop();
      if (auxiliary && user.auxiliaryTrack) user.auxiliaryTrack.stop();
      setSmallViewTrackMap((prev) => {
        if (auxiliary && prev[user.userId] === user.auxiliaryTrack) {
          delete prev[user.userId];
        }
        if (!auxiliary && prev[user.userId] === user.videoTrack) {
          delete prev[user.userId];
        } 
        return { ...prev };
      });
      return IClient.unsubscribe(user.userId, mediaType, auxiliary).then(() => {
        setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
        print(`unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}`);
      }).catch(e => {
        Toast.info(`unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`);
        print(`unsubscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(e)}`);
        throw e;
      });
    },
    [IClient],
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
    return IClient.batchSubscribe(subParams).then((batchSubscribeResult) => {
      for (const { error, track, uid: usrId, auxiliary } of batchSubscribeResult) {
        if (error) {
          Toast.info(
            `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          print(
            `subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'} failed: ${JSON.stringify(error)}`,
          );
          continue;
        }
        print(`subscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'}`);
        setSmallViewTrackMap((prev) => ({ ...prev, [usrId]: prev[usrId] || track as RemoteVideoTrack }));
        setRemoteChannelInfo((prev) => ({
          ...prev,
          remoteUsers: [...IClient.remoteUsers],
        }));
      }
      setRemoteChannelInfo((prev) => ({
        ...prev,
        subscribeAllVideo: true,
      }));
    });
  }, [IClient, remoteUsers]);

  const unsubscribeAllRemoteVideo = useCallback(() => {
    const unsubParams: SubscribeParam[] = [];
    for (const user of remoteUsers) {
      if (user.videoTrack) {
        if (mainViewTrack === user.videoTrack) {
          setMainView(null);
        }
        user.videoTrack.stop();
        unsubParams.push({ uid: user.userId, mediaType: 'video', auxiliary: false });
      }
      if (user.auxiliaryTrack) {
        if (mainViewTrack === user.auxiliaryTrack) {
          setMainView(null);
        }
        user.auxiliaryTrack.stop();
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
    return IClient.batchUnsubscribe(unsubParams).then(() => {
      for (const { uid: usrId, auxiliary } of unsubParams) {
        print(`unsubscribe user ${usrId} ${auxiliary ? 'screenShare' : 'camera'}`);
        setSmallViewTrackMap((prev) => ({ ...prev, [usrId]: undefined }));
        setRemoteChannelInfo((prev) => ({
          ...prev,
          remoteUsers: [...IClient.remoteUsers],
        }));
      }
      setRemoteChannelInfo((prev) => ({
        ...prev,
        subscribeAllVideo: false,
      }));
    });
  }, [IClient, remoteUsers, mainViewTrack]);

  const subscribeMCUAudio = useCallback(() => {
    return IClient.subscribe('mcu', 'audio').then((track) => {
      print(`subscribe mcu audio`);
      setRemoteChannelInfo((prev) => ({ ...prev, mcuAudioTrack: track as RemoteAudioTrack }));
      track.play();
      return track;
    }).catch(e => {
      Toast.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
      print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
      throw e;
    });
  }, [IClient]);

  const unsubscribeMCUAudio = useCallback(() => {
    mcuAudioTrack?.stop();
    return IClient.unsubscribe('mcu', 'audio').then(() => {
      print(`unsubscribe mcu audio`);
      setRemoteChannelInfo((prev) => ({ ...prev, mcuAudioTrack: null }));
    }).catch(e => {
      Toast.info(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
      print(`subscribe mcu audio failed: ${JSON.stringify(e)}`);
      throw e;
    });
  }, [mcuAudioTrack, IClient]);

  return {
    speakers,
    subscribe,
    unsubscribe,
    mcuAudioTrack,
    subscribeAllVideo,
    subscribeMCUAudio,
    unsubscribeAllRemoteVideo,
    subscribeAllRemoteVideo,
    unsubscribeMCUAudio,
  };
};
