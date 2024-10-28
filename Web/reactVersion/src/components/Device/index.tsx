import { Row, Space, Tooltip, Col, Divider, Typography } from 'dingtalk-design-desktop';
import Icon from '../Icon';
import CheckOutlined from 'dd-icons/es/CheckOutlined';
import { ReactNode, memo, useCallback, useMemo, useState } from 'react';
import { useRecoilState, useRecoilValue } from 'recoil';
import { constantConfig, deviceInfo } from '~/store';
import styles from './index.module.less';
import { useDevice } from '~/hooks/device';
import { useLocalChannel, useRemoteChannel } from '~/hooks/channel';
import { print } from '~/utils/tools';

const { Text } = Typography;

interface SelectorProps {
  title: string;
  list: MediaDeviceInfo[];
  currentId: string;
  onClick: (deviceId: string) => void;
}

interface DeviceProps {
  onClick?: () => void;
}

const Selector = memo((props: SelectorProps) => {
  const { title, list, currentId, onClick } = props;
  if (list.length === 0) return null;
  return (
    <Row className={styles.list}>
      <span>{title}</span>
      {list.map((item) => (
        <Col
          key={item.deviceId}
          onClick={() => {
            onClick(item.deviceId);
          }}
        >
          {currentId === item.deviceId ? <CheckOutlined className={styles.checked} /> : null}
          <Text>{item.label}</Text>
        </Col>
      ))}
    </Row>
  );
});

const useBase = () => {
  const [showPanel, setShowPannel] = useState(false);

  const getNode = useCallback(
    (pannel: ReactNode, icon: string, onClick: any, hideArrow = false) => {
      return (
        <Row className={styles.wrap}>
          <Space size={[12, 0]}>
            <Icon type={icon} className="deviceIcon" onClick={onClick} />
            {hideArrow ? null : <div onClick={(e) => e.stopPropagation()}>
              <Tooltip
                overlayClassName={styles.tooltip}
                overlayInnerStyle={{
                  backgroundColor: 'rgba(245,247,250,0.9)',
                }}
                onOpenChange={(show) => {
                  setShowPannel(show);
                }}
                open={showPanel}
                title={pannel}
                trigger={'click'}
              >
                <Icon
                  className={styles.arrow}
                  type={!showPanel ? 'iconXDS_Uparrow' : 'iconXDS_Downarrow'}
                />
              </Tooltip>
            </div>}
          </Space>
        </Row>
      );
    },
    [showPanel],
  );

  return {
    getNode,
    showPanel,
    setShowPannel,
  };
};

export const Camera = memo((props: DeviceProps) => {
  const { onClick } = props;
  const { cameraEnabled, cameraDimension, cameraFrameRate } = useDevice();
  const { publishedTracks, cameraTrack, client } = useLocalChannel();
  const [{ cameraList, cameraId }, setDeviceInfo] = useRecoilState(deviceInfo);
  const { getNode, setShowPannel } = useBase();
  const iconType = useMemo(() => {
    return !cameraEnabled ? 'iconXDS_FrameMeetingFill' : 'iconXDS_FrameMeetingLine';
  }, [cameraEnabled]);

  const DevicePannel = useMemo(() => {
    const onCameraClick = (deviceId: string) => {
      setDeviceInfo((prev) => ({ ...prev, cameraId: deviceId }));
      print(`camera changeTo ${deviceId}`);
      cameraTrack.setDevice(deviceId);
      setShowPannel(false);
    };
    return (
      <Row>
        <Selector
          title="请选择摄像头"
          list={cameraList}
          currentId={cameraId}
          onClick={onCameraClick}
        />
      </Row>
    );
  }, [
    cameraList,
    cameraId,
    cameraTrack,
    publishedTracks,
    client?.channelName,
    cameraDimension,
    cameraFrameRate,
  ]);
  return getNode(DevicePannel, iconType, onClick);
});

export const Mic = memo((props: DeviceProps) => {
  const { micEnabled } = useDevice();
  const { onClick } = props;
  const { isMobile } = useRecoilValue(constantConfig);
  const { publishedTracks, micTrack, client } = useLocalChannel();
  const [{ micList, speakerList, micId, speakerId }, setDeviceInfo] = useRecoilState(deviceInfo);
  const { subscribeMCUAudio, unsubscribeMCUAudio, mcuAudioTrack } = useRemoteChannel();
  const { getNode, setShowPannel, showPanel } = useBase();

  const iconType = useMemo(() => {
    return !micEnabled ? 'iconXDS_UnMute2Fill' : 'iconXDS_Mute2';
  }, [micEnabled]);

  const DevicePannel = useMemo(() => {
    const onMicClick = (deviceId: string) => {
      setDeviceInfo((prev) => ({ ...prev, micId: deviceId }));
      print(`mic changeTo ${deviceId}`);
      micTrack.setDevice(deviceId);
      setShowPannel(false);
    };
    const onSpeakerClick = (deviceId: string) => {
      setDeviceInfo((prev) => ({ ...prev, speakerId: deviceId }));
      print(`speaker changeTo ${deviceId}`);
      setShowPannel(false);
      mcuAudioTrack?.setSpeaker(deviceId || undefined);
    };
  
    return (
      <Row>
        <Selector title="请选择麦克风" list={micList} currentId={micId} onClick={onMicClick} />
        {isMobile ? null : (
          <>
            <Divider style={{ margin: '6px 0' }} />
            <Selector
              title="请选择扬声器"
              list={speakerList}
              currentId={speakerId}
              onClick={onSpeakerClick}
            />
          </>
        )}
      </Row>
    );
  }, [
    micList,
    speakerList,
    micId,
    speakerId,
    micTrack,
    showPanel,
    publishedTracks,
    client?.channelName,
    subscribeMCUAudio,
    unsubscribeMCUAudio,
    mcuAudioTrack,
  ]);

  return getNode(DevicePannel, iconType, onClick);
});

export const Screen = memo(() => {
  const { getNode } = useBase();
  return getNode(null, 'iconXDS_share_screen1', null, true);
});
