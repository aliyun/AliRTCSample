import { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Avatar, Col, Tooltip, List } from 'dingtalk-design-desktop';
import Icon from '~/components/Icon';
import { RemoteUser } from 'dingrtc';
import { downloadFileByBase64 } from '~/utils/tools';
import styles from '../index.module.less';
import classNames from 'classnames';
import { useRemoteChannel } from '~/hooks/channel';
import { useDevice } from '~/hooks/device';
import { useRecoilState, useRecoilValue } from 'recoil';
import { client, mainViewPrefer, remoteUserNetworks } from '~/store';
import { ScoreMap } from './NetWorkBar/Networks';
// import PlayResumer from '~/components/PlayResumer';

interface UserItemProps {
  user: RemoteUser;
  isLocal: boolean;
}

const SmallView = (props: UserItemProps) => {
  const { user, isLocal } = props;
  const { speakers, subscribe, unsubscribe } = useRemoteChannel();
  const rtcClient = useRecoilValue(client);
  const remoteUserNetwork = useRecoilValue(remoteUserNetworks);
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

  const currentTrack = useMemo(() => {
    if (mainPrefer.userId === user.userId) {
      return mainPrefer.prefer === 'camera' ? user.auxiliaryTrack : user.videoTrack;
    }
    return user.auxiliaryTrack || user.videoTrack;
  }, [mainPrefer, user.videoTrack, user.auxiliaryTrack]);

  useEffect(() => {
    if (!currentTrack) return;
    currentTrack.play(ref.current, { fit: 'cover' });
  }, [currentTrack]);

  const viewBigger = useCallback(() => {
    setMainPrefer((prev) => {
      if (prev.userId === user.userId) {
        return ({ userId: user.userId, prefer: prev.prefer === 'camera' ? 'auxiliary' : 'camera' })
      } else {
        return ({ userId: user.userId, prefer: 'camera' })
      }
    })
  }, [user]);

  const wrapClass = classNames({
    [styles.smallVideoItem]: true,
    [styles.avatar]: !videoIsPlay,
    [styles.camera]: videoIsPlay,
    [styles.speaking]: speakers.includes(user.userId),
  });

  const Actions = useMemo(() => {
    const buttons = [
      {
        text: '摄像头截图',
        show: user.videoTrack,
        onClick: () => {
          downloadFileByBase64(user.videoTrack.getCurrentFrameData(), user.userId);
        },
      },
      {
        text: '共享截图',
        show: user.auxiliaryTrack,
        onClick: () => {
          downloadFileByBase64(user.auxiliaryTrack.getCurrentFrameData(), user.userId);
        },
      },
      {
        text: '订阅摄像头',
        show: !isLocal && !user.videoTrack && user.hasVideo,
        onClick: () => {
          subscribe(user, 'video')
        },
      },
      {
        text: '取消订阅摄像头',
        show: !isLocal && user.videoTrack,
        onClick: () => {
          unsubscribe(user, 'video')
        },
      },
      {
        text: '订阅共享',
        show: !isLocal && !user.auxiliaryTrack && user.hasAuxiliary,
        onClick: () => {
          subscribe(user, 'video', true)
        },
      },
      {
        text: '取消订阅共享',
        show: !isLocal && user.auxiliaryTrack,
        onClick: () => {
          unsubscribe(user, 'video', true)
        },
      },
      {
        text: '切大流',
        show: !isLocal && user.videoTrack && streamType === 'low',
        onClick: () => {
          rtcClient.setRemoteVideoStreamType(user.userId, 'high').then(() => {
            setStreamType('high');
          });
        },
      },
      {
        text: '切小流',
        show: !isLocal && user.videoTrack && streamType === 'high',
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
      {/* <PlayResumer track={currentTrack} container={ref.current} /> */}
      <span className={styles.userId} onClick={(e) => e.stopPropagation()}>
        uid:{user.userId}
      </span>
      {isLocal ? null : ScoreMap(remoteUserNetwork[user.userId] || 0)?.icon}
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
};

export default SmallView;
