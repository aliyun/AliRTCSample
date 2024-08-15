import { memo, useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Avatar, Col, Tooltip, List, Modal } from 'dingtalk-design-desktop';
import Icon from '~/components/Icon';
import { RemoteUser } from 'dingrtc';
import { downloadFileByBase64, isIOS, isWeixin } from '~/utils/tools';
import styles from '../index.module.less';
import classNames from 'classnames';
import { useRemoteChannel } from '~/hooks/channel';
import { useDevice } from '~/hooks/device';
import { useRecoilState, useRecoilValue } from 'recoil';
import { client, mainViewPrefer } from '~/store';


interface UserItemProps {
  user: RemoteUser;
  isLocal: boolean;
}

const SmallView = memo((props: UserItemProps) => {
  const { user, isLocal } = props;
  const { speakers } = useRemoteChannel();
  const rtcClient = useRecoilValue(client);
  const [showActions, setShowActions] = useState(false);
  const [mainPrefer, setMainPrefer] = useRecoilState(mainViewPrefer);
  const { cameraEnabled, micEnabled } = useDevice();
  const [streamType, setStreamType] = useState('high');
  const ref = useRef<HTMLDivElement>(null);

  const micIconEnable = useMemo(() => {
    if (isLocal) return micEnabled;
    return user?.hasAudio && !user?.audioMuted;
  }, [isLocal, user?.hasAudio, user?.audioMuted, micEnabled]);

  const videoIsPlay = useMemo(() => {
    let track;
    if (mainPrefer.userId === user.userId) {
      track = mainPrefer.prefer === 'auxiliary' ? user.videoTrack : user.auxiliaryTrack
    } else {
      track = user.auxiliaryTrack || user.videoTrack;
    }
    if (!track) return false;
    if (track === user.videoTrack)
      return isLocal ? cameraEnabled : !user?.videoMuted;
    if (track === user.auxiliaryTrack)
      return isLocal || !user.auxiliaryMuted;
  }, [
    mainPrefer,
    user?.videoTrack?.isPlaying,
    isLocal,
    cameraEnabled,
    user?.videoMuted,
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
      },
      onCancel() {
        track.play(ref.current, { fit: 'cover' });
      }
    })
  }, []);

  const currentTrack = useMemo(() => {
    if (mainPrefer.userId === user.userId) {
      return mainPrefer.prefer === 'camera' ? user.auxiliaryTrack : user.videoTrack;
    }
    return user.auxiliaryTrack || user.videoTrack;
  }, [mainPrefer, user.videoTrack, user.auxiliaryTrack]);

  useEffect(() => {
    if (!currentTrack) return;
    if (isIOS() && isWeixin()) {
      playTrack(currentTrack);
    } else {
      currentTrack.play(ref.current, { fit: 'cover' });
    }
  }, [currentTrack]);

  const isCamera = useMemo(() => {
    const base = currentTrack && currentTrack === user.videoTrack;
    if (isLocal) return base && cameraEnabled;
    return base;
  }, [currentTrack, user.videoTrack, isLocal, cameraEnabled]);
  const isScreen = useMemo(
    () => currentTrack && currentTrack === user.auxiliaryTrack,
    [currentTrack, user.auxiliaryTrack],
  );

  const viewBigger = useCallback(() => {
    setMainPrefer((prev) => {
      if (prev.userId === user.userId) {
        return ({ userId: user.userId, prefer: prev.prefer === 'camera' ? 'auxiliary' : 'camera'})
      } else {
        return ({ userId: user.userId, prefer: 'camera' })
      }
    })
  }, [user, isScreen]);

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
          rtcClient.setRemoteVideoStreamType(user.userId, 'high').then(() => {
            setStreamType('high');
          });
        },
      },
      {
        text: '切小流',
        show: !isLocal && isCamera && streamType === 'high',
        onClick: () => {
          rtcClient.setRemoteVideoStreamType(user.userId, 'low').then(() => {
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
        {user?.auxiliaryTrack ? (
          <Icon style={{ color: 'limegreen' }} type="iconXDS_share_screen1" />
        ) : !micIconEnable ? <Icon type="iconXDS_UnMute2Fill" /> : <Icon type="iconXDS_Mute2" />}
        <Tooltip title={user.userName}>
          <span>{user.userName}</span>
        </Tooltip>
      </div>
      {Actions}
    </Col>
  );
});

export default SmallView;
