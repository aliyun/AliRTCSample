import {
  LocalTrack,
  LocalVideoTrack,
  RemoteAudioTrack,
  RemoteUser,
  RemoteVideoTrack,
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
  };
};

export const useRemoteChannel = () => {
  const [{ mcuAudioTrack, speakers }, setRemoteChannelInfo] = useRecoilState(remoteChannelInfo);
  const IClient = useRecoilValue(client);
  const setSmallViewTrackMap = useSetRecoilState(smallViewTrackMap);

  const subscribe = useCallback(
    (user: RemoteUser, mediaType: TrackMediaType, auxiliary?: boolean) => {
      return IClient.subscribe(user.userId, mediaType, auxiliary).then((track: any) => {
        print(`subscribe user ${user.userId} ${auxiliary ? 'screenShare' : 'camera'}}`);
        setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
        setSmallViewTrackMap((prev) => ({ ...prev, [user.userId]: prev[user.userId] || track }));
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
    subscribeMCUAudio,
    unsubscribeMCUAudio,
  };
};
