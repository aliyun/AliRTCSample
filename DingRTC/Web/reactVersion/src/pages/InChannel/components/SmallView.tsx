import { memo, useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Avatar, Col, Tooltip, List, Modal } from 'dingtalk-design-desktop';
import Icon from '~/components/Icon';
import { RemoteUser } from 'dingrtc';
import { downloadFileByBase64, isIOS, isWeixin } from '~/utils/tools';
import styles from '../index.module.less';
import classNames from 'classnames';
import { useMainView, useRemoteChannel } from '~/hooks/channel';
import { useDevice } from '~/hooks/device';
import { useRecoilState, useRecoilValue } from 'recoil';
import { client, smallViewTrackMap } from '~/store';

interface UserItemProps {
  user: RemoteUser;
  isLocal: boolean;
}

const SmallView = memo((props: UserItemProps) => {
  const { user, isLocal } = props;
  const { subscribe, speakers, subscribeAllVideo } = useRemoteChannel();
  const IClient = useRecoilValue(client);
  const [showActions, setShowActions] = useState(false);
  const [viewMap, setViewMap] = useRecoilState(smallViewTrackMap);
  const { updateMainView } = useMainView();
  const { cameraEnabled, micEnabled } = useDevice();
  const [streamType, setStreamType] = useState('high');
  const ref = useRef<HTMLDivElement>(null);
  const cameraIconEnable = useMemo(() => {
    if (isLocal) return cameraEnabled;
    return user?.hasVideo && !user?.videoMuted;
  }, [isLocal, user?.hasVideo, user?.videoMuted, cameraEnabled]);

  const micIconEnable = useMemo(() => {
    if (isLocal) return micEnabled;
    return user?.hasAudio && !user?.audioMuted;
  }, [isLocal, user?.hasAudio, user?.audioMuted, micEnabled]);

  const videoIsPlay = useMemo(() => {
    const track = viewMap[user.userId];
    if (!track) return false;
    if (track === user.videoTrack)
      return isLocal ? cameraEnabled : !user?.videoMuted;
    if (track === user.auxiliaryTrack)
      return isLocal || !user.auxiliaryMuted;
  }, [
    user?.videoTrack?.isPlaying,
    isLocal,
    cameraEnabled,
    user?.videoMuted,
    viewMap,
    user?.auxiliaryTrack,
    user?.auxiliaryMuted,
  ]);

  const playTrack = useCallback((track: any) => {
    Modal.centerConfirm({
      title: '提示',
      content: 'iOS微信浏览器需要在回调内播放',
      okText: '确认',
      onOk() {
        track.play(ref.current, { fit: 'cover' });
        setViewMap((prev) => ({ ...prev, [user.userId]: track }));
      },
      onCancel() {
        track.play(ref.current, { fit: 'cover' });
        setViewMap((prev) => ({ ...prev, [user.userId]: track }));
      }
    })
  }, []);

  const currentTrack = useMemo(() => viewMap[user.userId], [viewMap])

  useEffect(() => {
    if (currentTrack && !currentTrack.isPlaying) {
      if (isIOS() && isWeixin()) {
        playTrack(currentTrack);
      } else {
        currentTrack.play(ref.current, { fit: 'cover' });
        setViewMap((prev) => ({ ...prev, [user.userId]: currentTrack }));
      }
    }
  }, [currentTrack?.isPlaying]);

  const isCamera = useMemo(() => {
    const base = viewMap[user.userId] === user.videoTrack && !!viewMap[user.userId];
    if (isLocal) return base && cameraEnabled;
    return base;
  }, [viewMap, user, isLocal, cameraEnabled]);
  const isScreen = useMemo(
    () => viewMap[user.userId] === user.auxiliaryTrack && !!viewMap[user.userId],
    [viewMap, user],
  );

  const viewBigger = useCallback(() => {
    const isEmpty = !isCamera && !isScreen;
    if ((isEmpty || isScreen) && user.hasAuxiliary) {
      if (isLocal) {
        updateMainView(user, user.auxiliaryTrack);
      } else if (user.auxiliaryTrack) {
        updateMainView(user, user.auxiliaryTrack);
      } else if (subscribeAllVideo) {
        subscribe(user, 'video', true);
      }
      return;
    }
    if ((isCamera || isEmpty) && user.hasVideo && user.videoTrack) {
      updateMainView(user, user.videoTrack);
    }
  }, [user, isLocal, updateMainView, isCamera, isScreen, subscribeAllVideo]);

  const wrapClass = classNames({
    [styles.smallVideoItem]: true,
    [styles.avatar]: !videoIsPlay,
    [styles.camera]: videoIsPlay,
    [styles.speaking]: speakers.includes(user.userId),
  });

  const Actions = useMemo(() => {
    const buttons = [
      {
        text: '截图',
        show: isCamera || isScreen,
        onClick: () => {
          let dataURL = '';
          if (isCamera) dataURL = user.videoTrack.getCurrentFrameData();
          if (isScreen) dataURL = user.auxiliaryTrack.getCurrentFrameData();
          downloadFileByBase64(dataURL, user.userId);
        },
      },
      {
        text: '切大流',
        show: !isLocal && isCamera && streamType === 'low',
        onClick: () => {
          IClient.setRemoteVideoStreamType(user.userId, 'high').then(() => {
            setStreamType('high');
          });
        },
      },
      {
        text: '切小流',
        show: !isLocal && isCamera && streamType === 'high',
        onClick: () => {
          IClient.setRemoteVideoStreamType(user.userId, 'low').then(() => {
            setStreamType('low');
          });
        },
      },
    ].filter((item) => item.show);
    if (!buttons.length) return null;
    return (
      <Tooltip
        trigger="click"
        overlayInnerStyle={{
          backgroundColor: 'rgba(245, 247, 250, 0.9)',
        }}
        open={showActions}
        onOpenChange={(show) => setShowActions(show)}
        title={
          <List>
            {buttons.map((item) => (
              <List.Item
                style={{ cursor: 'pointer' }}
                key={item.text}
                onClick={() => {
                  item.onClick();
                  setShowActions(false);
                }}
              >
                {item.text}
              </List.Item>
            ))}
          </List>
        }
      >
        <div className={styles.smallViewActions}>
          <Icon type="iconXDS_List" />
        </div>
      </Tooltip>
    );
  }, [
    isCamera,
    isScreen,
    isLocal,
    showActions,
    user?.videoTrack,
    user?.auxiliaryTrack,
    streamType,
    user?.hasAuxiliary,
    user?.hasVideo,
  ]);

  return (
    <Col ref={ref} onDoubleClick={viewBigger} className={wrapClass}>
      <span className={styles.userId} onClick={(e) => e.stopPropagation()}>
        uid:{user.userId}
      </span>
      <Avatar size="large">{user.userName}</Avatar>
      <div className={styles.smallViewStatus}>
        {!micIconEnable ? <Icon type="iconXDS_UnMute2Fill" /> : <Icon type="iconXDS_Mute2" />}
        {!cameraIconEnable ? (
          <Icon type="iconXDS_FrameMeetingFill" />
        ) : (
          <Icon type="iconXDS_FrameMeetingLine" />
        )}
        {user?.auxiliaryTrack || user?.hasAuxiliary ? (
          <Icon style={{ color: 'limegreen' }} type="iconXDS_share_screen1" />
        ) : null}
      </div>
      {Actions}
    </Col>
  );
});

export default SmallView;
