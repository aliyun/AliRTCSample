import { BaseSyntheticEvent, memo, useRef, useState } from 'react';
import { Button, Avatar, Col } from 'dingtalk-design-desktop';
import Icon from './Icon';
import { DingRTCClient, RemoteUser, SimpleLogger } from 'dingrtc';
import { downloadFileByBase64, isMobile } from '../utils/tools';

interface UserStatus {
  subscribed?: boolean;
  cameraEnabled?: boolean;
  micEnabled?: boolean;
}

interface UserItemProps {
  user: RemoteUser;
  status: UserStatus;
  isLocal: boolean;
  client: DingRTCClient;
  onDoubleClick: () => void;
  updateStatus: (userId: string, val: UserStatus) => void;
}

const logger = SimpleLogger.getLogger('DEMOAPP');

const UserItem = memo((props: UserItemProps) => {
  const { user, status, client, updateStatus, onDoubleClick, isLocal } = props;
  const [loading, setLoading] = useState(false);
  const [streamType, setStreamType] = useState('high');
  const ref = useRef<HTMLDivElement>(null);
  const cut = () => {
    const dataURL = user.videoTrack.getCurrentFrameData();
    downloadFileByBase64(dataURL, user.userId)
  }

  const toggleStreamType = (e: BaseSyntheticEvent) => {
    e.stopPropagation();
    const newType = streamType === 'high' ? 'low' : 'high';
    client.setRemoteVideoStreamType(user.userId, newType).then(() => {
      setStreamType(newType);
    });
  };
  
  const toggleSub = (e: BaseSyntheticEvent) => {
    e.stopPropagation();
    setLoading(true);
    if (status.subscribed) {
      user.videoTrack.stop();
      client
        .unsubscribe(user.userId, 'video')
        .then(() => {
          logger.info(`unsubscribe user ${user.userId} video`);
          updateStatus(user.userId, { subscribed: false });
          setLoading(false);
        })
        .catch(() => {
          setLoading(false);
        });
    } else {
      client
        .subscribe(user.userId, 'video')
        .then((track) => {
          setLoading(false);
          logger.info(`subscribe user ${user.userId} video`);
          updateStatus(user.userId, { cameraEnabled: true, subscribed: true });
          track.play(`#uid${user.userId}`);
        })
        .catch(() => {
          setLoading(false);
        });
    }
  };
  return (
    <Col
      ref={ref}
      onDoubleClick={onDoubleClick}
      id={`uid${user.userId}`}
      className='small-video-item'
    >
      {!(status?.subscribed || status?.cameraEnabled) ? (
        <Avatar size="large">{user.userName}</Avatar>
      ) : null}
      <div className='small-video-item-status'>
        {!status?.micEnabled ? <Icon type="iconXDS_UnMute2Fill" /> : <Icon type="iconXDS_Mute2" />}
        {!status?.cameraEnabled ? (
          <Icon type="iconXDS_FrameMeetingFill" />
        ) : (
          <Icon type="iconXDS_FrameMeetingLine" />
        )}
        {user?.auxiliaryTrack || user?.hasAuxiliary ? <Icon type="iconXDS_share_screen1" /> : null}
      </div>
      {status?.cameraEnabled && !isLocal ? (
        <Button
          size="small"
          disabled={loading}
          type="primary"
          className='subBtn'
          onClickCapture={toggleSub}
        >
          {status?.subscribed ? '取消订阅' : '订阅'}
        </Button>
      ) : null}
      {(!isMobile() && (status?.subscribed || status?.cameraEnabled))  ? <Button size="small" className='cutBtn' onClick={cut}>截图</Button> : null}
      {status?.cameraEnabled && status?.subscribed && !isLocal ? (
        <Button
          size="small"
          className='streamTypeBtn'
          onClickCapture={toggleStreamType}
        >
          {streamType === 'low' ? '大' : '小'}流
        </Button>
      ) : null}
    </Col>
  );
});

export default UserItem;
