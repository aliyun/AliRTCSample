import { memo, useCallback, useEffect, useRef, useState } from 'react';
import DingRTC, { RemoteUser, RemoteStreamType, TrackMediaType } from 'dingrtc';
import { Space, Row, Toast } from 'dingtalk-design-desktop';
import SmallView from './components/SmallView';
import { print, parseTime } from '~/utils/tools';
import { useRecoilState, useRecoilValue, useResetRecoilState, useSetRecoilState } from 'recoil';
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
  const { timeLeft } = useRecoilValue(localChannelInfo);
  const [time, setTime] = useState(timeLeft - 1);
  useEffect(() => {
    const timer = setInterval(() => {
      setTime((currTime) => Math.max(currTime - 1, 0));
    }, 1000);
    return () => {
      clearInterval(timer);
    };
  }, []);
  return <span>剩余时长：{parseTime(time)}</span>;
});

const Index = memo(() => {
  const setGlobalFlag = useSetRecoilState(globalFlag);
  const [{ cameraTrack, micTrack, screenTrack, timeLeft }, setLocalChannelInfo] = useRecoilState(localChannelInfo);
  const IClient = useRecoilValue(client);
  const { userId, userName } = useRecoilValue(currentUserInfo);
  const { subscribe, unsubscribe, subscribeAllVideo } = useRemoteChannel();
  const setMainView = useSetRecoilState(mainView);
  const resetLocalChannelInfo = useResetRecoilState(localChannelInfo);
  const resetRemoteChannelInfo = useResetRecoilState(remoteChannelInfo);
  const setSmallVideMap = useSetRecoilState(smallViewTrackMap);
  const [{ remoteUsers, mcuAudioTrack }, setRemoteChannelInfo] = useRecoilState(remoteChannelInfo);
  const otherVideosWrapperRef = useRef<HTMLDivElement>(null);
  const customVideo = useRef<HTMLVideoElement>(null);

  useEffect(() => {
    if (!customVideo.current) return;
    customVideo.current.onplay = async () => {
      // @ts-ignore
      const videoTrack = customVideo.current.captureStream().getVideoTracks()[0];
      const customTrack = await DingRTC.createCustomVideoTrack({
        mediaStreamTrack: videoTrack,
        frameRate: 45,
        dimension: 'VD_1920x1080',
      });
      // setCustomVideoTrak(customTrack);
      IClient.publish(customTrack);
    };
  }, []);

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
    resetLocalChannelInfo();
    setMainView(null);
    resetRemoteChannelInfo();
    setSmallVideMap({});
    setGlobalFlag({
      joined: false,
    });
  }, [mcuAudioTrack, remoteUsers, micTrack, cameraTrack, screenTrack, screenTrack, IClient]);
  useEffect(() => {
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
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
    });
    return () => {
      IClient.removeAllListeners();
    };
  }, []);

  useEffect(() => {
    const onUserPublished = (user: RemoteUser, mediaType: TrackMediaType, auxiliary: boolean) => {
      print(`user ${user.userId} published ${auxiliary ? 'screenShare' : mediaType}}`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
      if (mediaType !== 'video') {
        return;
      }
      if (subscribeAllVideo) {
        subscribe(user, 'video', auxiliary);
      }
    };
    const onUserUnpublished = (user: RemoteUser, mediaType: TrackMediaType, auxiliary: boolean) => {
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...IClient.remoteUsers] }));
      if (mediaType !== 'video') {
        return;
      }
      if (subscribeAllVideo) {
        unsubscribe(user, 'video', auxiliary);
      }
    };
    IClient.on('user-unpublished', onUserUnpublished);
    IClient.on('user-published', onUserPublished);
    return () => {
      IClient.off('user-unpublished', onUserUnpublished);
      IClient.off('user-published', onUserPublished);
    };
  }, [subscribeAllVideo]);

  useEffect(() => {
    if (!screenTrack) return;
    const clearTrack = () => {
      screenTrack.stop();
      IClient.unpublish(screenTrack);
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
        {timeLeft ? <Timer /> : null}
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
