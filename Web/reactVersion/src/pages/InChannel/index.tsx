import { memo, useCallback, useEffect, useState } from 'react';
import {
  RemoteUser,
  RemoteStreamType,
  TrackMediaType,
  NetworkQuality,
} from 'dingrtc';
import { Space, Row, Toast } from 'dingtalk-design-desktop';
import SmallView from './components/SmallView';
import { print, parseTime } from '~/utils/tools';
import { useRecoilState, useRecoilValue, useResetRecoilState, useSetRecoilState } from 'recoil';
import {
  client,
  globalFlag,
  localChannelInfo,
  mainViewPrefer,
  remoteChannelInfo,
} from '~/store';
import ToolBar from './components/ToolBar';
import styles from './index.module.less';
import { useLocalChannel, useNetworkStats, useRemoteChannel } from '~/hooks/channel';
import MainView from './components/MainView';
import { NetworkDetector } from './components/NetWorkBar/Networks';
import Icon from '~/components/Icon';

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
  const rtcClient = useRecoilValue(client);
  const [fullscreen, setFullScreen] = useState(false);
  const { allUsers, cameraTrack, micTrack, screenTrack, timeLeft, setLocalChannelInfo, userId } = useLocalChannel();
  const { subscribe, unsubscribe, subscribeAllVideo } = useRemoteChannel();
  const resetLocalChannelInfo = useResetRecoilState(localChannelInfo);
  const resetMainViewPrefer = useResetRecoilState(mainViewPrefer);
  const resetRemoteChannelInfo = useResetRecoilState(remoteChannelInfo);
  const [{ remoteUsers, mcuAudioTrack }, setRemoteChannelInfo] = useRecoilState(remoteChannelInfo);
  const { getRtcStats } = useNetworkStats();

  const onFullScreen = useCallback(() => {
    if (!document.fullscreenElement) {
      setFullScreen(true);
      document.documentElement.requestFullscreen();
    } else {
      if (document.exitFullscreen) {
        setFullScreen(false);
        document.exitFullscreen();
      }
    }
  }, [])

  const clearRoom = useCallback(() => {
    if (cameraTrack) {
      cameraTrack.close();
    }
    if (document.fullscreenElement) {
      document.exitFullscreen();
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
    resetRemoteChannelInfo();
    resetMainViewPrefer();
    setGlobalFlag({
      joined: false,
      hideToolBar: false,
    });
  }, [mcuAudioTrack, remoteUsers, micTrack, cameraTrack, screenTrack, screenTrack, rtcClient]);
  useEffect(() => {
    rtcClient.on('user-joined', (user: RemoteUser) => {
      print(`user ${user.userId} joined`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...rtcClient.remoteUsers] }));
    });
    rtcClient.on('stream-type-changed', (uid: string, streamType: RemoteStreamType) => {
      print(`user ${uid} streamType changeTo ${streamType}`);
    });

    rtcClient.on('connection-state-change', (current, _, reason) => {
      print(`connection-state-change ${current} ${reason || ''}`);
      if (current === 'disconnected') {
        if (reason !== 'leave') {
          Toast.info(reason);
        }
        clearRoom();
      }
    });
    rtcClient.on('network-quality', (uplink: NetworkQuality, downlink: NetworkQuality) => {
      setLocalChannelInfo((prev) => ({ ...prev, networkQuality: uplink > downlink ? uplink : downlink }));
    });
    rtcClient.on('volume-indicator', (uids: string[]) => {
      if (uids.length) {
        print(`${uids.join()} is speaking`);
      }
      setRemoteChannelInfo((prev) => ({ ...prev, speakers: uids }));
    });
    rtcClient.on('user-info-updated', (uid, msg) => {
      print(`user ${uid}: ${msg}`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...rtcClient.remoteUsers] }));
    });
    rtcClient.on('user-left', (user: RemoteUser) => {
      print(`user ${user.userId} left`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...rtcClient.remoteUsers] }));
    });
    const onExitFullScreen = () => {
      if (!document.fullscreenElement) setFullScreen(false);
    }
    document.addEventListener('fullscreenchange', onExitFullScreen);
    return () => {
      document.removeEventListener('fullscreenchange', onExitFullScreen);
      rtcClient.removeAllListeners();
    };
  }, []);

  useEffect(() => {
    const onUserPublished = (user: RemoteUser, mediaType: TrackMediaType, auxiliary: boolean) => {
      print(`user ${user.userId} published ${mediaType === 'audio' ? 'audio' : auxiliary ? 'screenShare' : mediaType}}`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...rtcClient.remoteUsers] }));
      if (mediaType !== 'video') {
        return;
      }
      if (subscribeAllVideo) {
        subscribe(user, 'video', auxiliary);
      }
    };
    const onUserUnpublished = (user: RemoteUser, mediaType: TrackMediaType, auxiliary: boolean) => {
      print(`user ${user.userId} unpublished ${mediaType === 'audio' ? 'audio' : auxiliary ? 'screenShare' : mediaType}}`);
      setRemoteChannelInfo((prev) => ({ ...prev, remoteUsers: [...rtcClient.remoteUsers] }));
    };
    rtcClient.on('user-unpublished', onUserUnpublished);
    rtcClient.on('user-published', onUserPublished);
    return () => {
      rtcClient.off('user-unpublished', onUserUnpublished);
      rtcClient.off('user-published', onUserPublished);
    };
  }, [subscribeAllVideo, subscribe, unsubscribe]);

  useEffect(() => {
    if (!screenTrack) return;
    const clearTrack = () => {
      screenTrack.stop();
      rtcClient.unpublish(screenTrack);
      setLocalChannelInfo((prev) => ({ ...prev, screenTrack: null }));
    };
    screenTrack.on('track-ended', clearTrack);
    return () => {
      screenTrack.off('track-ended', clearTrack);
    };
  }, [screenTrack]);

  useEffect(() => {
    getRtcStats();
    const timer = setInterval(() => {
      getRtcStats();
    }, 2000);
    return () => {
      clearInterval(timer);
    };
  }, [getRtcStats]);

  return (
    <Row className={styles.blockWrapper}>
      <Row>
        {timeLeft ? <Timer /> : null}
        <NetworkDetector />
        <Row className={styles.fullscreen} onClick={onFullScreen}><Icon type={fullscreen ? 'iconXDS_Minimize' : 'iconXDS_FullScreen'} />{fullscreen ? '退出全屏' : '全屏'}</Row>
      </Row>
      <Space size={8} className={styles.smallVideoItems}>
        {allUsers.map((user) => (
          <SmallView
            isLocal={user.userId === userId}
            user={user as RemoteUser}
            key={user.userId}
          />
        ))}
      </Space>
      <MainView />
      <ToolBar onLeave={clearRoom} />
    </Row>
  );
});

export default memo(Index);
