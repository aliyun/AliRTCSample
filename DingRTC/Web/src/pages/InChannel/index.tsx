import { memo, useCallback, useEffect, useRef, useState } from 'react';
import {
  RemoteUser,
  RemoteStreamType,
  TrackMediaType,
} from 'dingrtc';
import { Space, Row, Toast } from 'dingtalk-design-desktop';
import SmallView from './components/SmallView';
import { parseTime, print } from '~/utils/tools';
import { useRecoilState, useRecoilValue, useSetRecoilState } from 'recoil';
import {
  client,
  currentUserInfo,
  globalFlag,
  localChannelInfo,
  mainView,
  remoteChannelInfo,
  smallViewTrackMap,
} from '~/store';
import ToolBar from './components/ToolBar';
import styles from './index.module.less';
import { useRemoteChannel } from '~/hooks/channel';
import MainView from './components/MainView';

const Timer = memo(() => {
  const [confTime, setConfTime] = useState('0:00');
  useEffect(() => {
    const startTime = Date.now();
    const timer = setInterval(() => {
      const currentTime = Date.now();
      const duration = currentTime - startTime;
      setConfTime(parseTime(duration))
    }, 1000)
    return () => {
      clearInterval(timer);
    }
  }, []);
  return (
    <span>{confTime}</span>
  );
});

const Index = memo(() => {
  const setGlobalFlag = useSetRecoilState(globalFlag);
  const [{ cameraTrack, micTrack, screenTrack }, setLocalChannelInfo] =
    useRecoilState(localChannelInfo);
  const IClient = useRecoilValue(client);
  const { userId, userName } = useRecoilValue(currentUserInfo);
  const { subscribe, unsubscribe } = useRemoteChannel();
  const setMainView = useSetRecoilState(mainView);
  const setSmallVideMap = useSetRecoilState(smallViewTrackMap);
  const [{ remoteUsers, mcuAudioTrack }, setRemoteChannelInfo] = useRecoilState(remoteChannelInfo);
  const otherVideosWrapperRef = useRef<HTMLDivElement>(null);

  const clearRoom = useCallback(() => {
    if (cameraTrack) {
      cameraTrack.close();
    }
    if (micTrack) {
      micTrack.close();
    }
    if (screenTrack) {
      screenTrack.close();
    }
    if (mcuAudioTrack) {
      mcuAudioTrack.stop();
    }
    
    remoteUsers.forEach((user) => {
      if (user.auxiliaryTrack) {
        user.auxiliaryTrack.stop();
      }
      if (user.videoTrack) {
        user.videoTrack.stop();
      }
    });
    setLocalChannelInfo({
      cameraTrack: null,
      micTrack: null,
      screenTrack: null,
      publishedTracks: [],
    });
    setMainView(null);
    setRemoteChannelInfo({
      mcuAudioTrack: null,
      remoteUsers: [],
      speakers: [],
    });
    setSmallVideMap({});
    setGlobalFlag({
      joined: false,
      showLog: false,
    });
  }, [mcuAudioTrack, remoteUsers, micTrack, cameraTrack, screenTrack, screenTrack, IClient]);
  useEffect(() => {
    if (!IClient) return;
    IClient.on('user-joined', (user: RemoteUser) => {
      print(`user ${user.userId} joined`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
    });
    IClient.on('stream-type-changed', (uid: string, streamType: RemoteStreamType) => {
      print(`user ${uid} streamType changeTo ${streamType}`);
    });

    IClient.on('connection-state-change', (current, _, reason) => {
      print(`connection-state-change ${current} ${reason || ''}`);
      if (current === 'disconnected') {
        Toast.info(reason);
        clearRoom();
      }
    });
    IClient.on('user-published', (user, mediaType, auxiliary) => {
      print(`user ${user.userId} published ${auxiliary ? 'screenShare' : mediaType}}`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
      if (mediaType !== 'video') {
        return;
      }
      subscribe(user, 'video', auxiliary);
    });
    IClient.on(
      'user-unpublished',
      (user: RemoteUser, mediaType: TrackMediaType, auxiliary: boolean) => {
        setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
        if (mediaType !== 'video') {
          return;
        }
        unsubscribe(user, 'video', auxiliary);
      },
    );
    IClient.on('volume-indicator', (uids: string[]) => {
      if (uids.length) {
        print(`${uids.join()} is speaking`);
      }
      setRemoteChannelInfo((prev) => ({ ...prev, speakers: uids }));
    });
    IClient.on('user-info-updated', (uid, msg) => {
      print(`user ${uid}: ${msg}`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
    });
    IClient.on('user-left', (user: RemoteUser) => {
      print(`user ${user.userId} left`);
      if (user.videoTrack) {
        unsubscribe(user, 'video');
      }
      if (user.auxiliaryTrack) {
        unsubscribe(user, 'video', true);
      }
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
    });
    return () => {
      IClient.removeAllListeners();
    };
  }, [IClient]);

  useEffect(() => {
    if (!screenTrack) return;
    const clearTrack = () => {
      screenTrack.stop();
      setLocalChannelInfo((prev) => ({ ...prev, screenTrack: null }));
    };
    screenTrack.on('track-ended', clearTrack);
    return () => {
      screenTrack.off('track-ended', clearTrack);
    };
  }, [screenTrack]);

  return (
    <Space direction="vertical" className={styles.blockWrapper}>
      <Row ref={otherVideosWrapperRef} className={styles.videoWrapper}>
        <Timer />
        <Space size={8} className={styles.smallVideoItems}>
          {[
            {
              userId,
              videoTrack: cameraTrack,
              userName,
              hasVideo: !!cameraTrack,
              videoMuted: !cameraTrack?.enabled,
              auxiliaryMuted: !screenTrack?.enabled,
              hasAuxiliary: !!screenTrack,
              auxiliaryTrack: screenTrack,
            },
            ...remoteUsers,
          ].map((user) => (
            <SmallView
              isLocal={user.userId === userId}
              user={user as RemoteUser}
              key={user.userId}
            />
          ))}
        </Space>
        <MainView />
      </Row>
      <ToolBar onLeave={clearRoom} />
    </Space>
  );
});

export default memo(Index);
