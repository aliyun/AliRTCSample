import { Row, Space, Tooltip, Col, Divider, Typography, Toast } from 'dingtalk-design-desktop';
import Icon from '../Icon';
import CheckOutlined from 'dd-icons/es/CheckOutlined';
import { ReactNode, memo, useCallback, useMemo, useState } from 'react';
import { useRecoilState, useRecoilValue, useSetRecoilState } from 'recoil';
import { constantConfig, deviceInfo } from '~/store';
import styles from './index.module.less';
import { useDevice } from '~/hooks/device';
import { useLocalChannel, useRemoteChannel } from '~/hooks/channel';
import VideoEncoder from '~/components/Device/VideoEncoder';
import { VideoDimension } from 'dingrtc';
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
    (pannel: ReactNode, icon: string, onClick: any) => {
      return (
        <Row className={styles.wrap}>
          <Space size={[12, 0]}>
            <Icon type={icon} className="deviceIcon" onClick={onClick} />
            <div onClick={(e) => e.stopPropagation()}>
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
            </div>
          </Space>
        </Row>
      );
    },
    [showPanel],
  );

  return {
    setShowPannel,
    getNode,
  };
};

export const Camera = memo((props: DeviceProps) => {
  const { onClick } = props;
  const { cameraEnabled, cameraDimension, cameraFrameRate } = useDevice();
  const { publishedTracks, cameraTrack, client, publish, unpublish } = useLocalChannel();
  const [{ cameraList, cameraId }, setDeviceInfo] = useRecoilState(deviceInfo);
  const { getNode, setShowPannel } = useBase();
  const [showEncoderModal, setShowEncoderModal] = useState(false);
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
    const isCameraPublish = publishedTracks.includes(cameraTrack);
    const onCameraPubClick = () => {
      isCameraPublish ? unpublish([cameraTrack]) : publish([cameraTrack]);
      setShowPannel(false);
    };
    const updateEncoder = (frameRate: number, dimension: VideoDimension) => {
      cameraTrack.setEncoderConfiguration({
        frameRate,
        dimension,
      });
      setDeviceInfo((prev) => ({
        ...prev,
        cameraFrameRate: frameRate,
        cameraDimension: dimension,
      }));
    };
    return (
      <Row>
        <>
          <Selector
            title="请选择摄像头"
            list={cameraList}
            currentId={cameraId}
            onClick={onCameraClick}
          />
          {client?.channelName ? (
            <>
              <Divider style={{ margin: '6px 0' }} />
              <Row className={styles.list}>
                <span>操作</span>
                <div onClick={onCameraPubClick}>{isCameraPublish ? '取消发布' : '发布'}</div>
                <div
                  onClick={() => {
                    if (isCameraPublish) {
                      Toast.info('推流过程中不能调整编码');
                      return;
                    }
                    setShowEncoderModal(true);
                    setShowPannel(false);
                  }}
                >
                  调整视频编码
                </div>
              </Row>
              <VideoEncoder
                open={showEncoderModal}
                frameRate={cameraFrameRate}
                dimension={cameraDimension}
                onOk={updateEncoder}
                onClose={() => setShowEncoderModal(false)}
              />
            </>
          ) : null}
        </>
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
    showEncoderModal,
  ]);
  const renderNode = useMemo(
    () => getNode(DevicePannel, iconType, onClick),
    [getNode, DevicePannel, iconType, onClick],
  );
  return renderNode;
});

export const Mic = memo((props: DeviceProps) => {
  const { micEnabled } = useDevice();
  const { onClick } = props;
  const { isMobile } = useRecoilValue(constantConfig);
  const { publishedTracks, micTrack, client, publish, unpublish } = useLocalChannel();
  const [{ micList, speakerList, micId, speakerId }, setDeviceInfo] = useRecoilState(deviceInfo);
  const { subscribeMCUAudio, unsubscribeMCUAudio, mcuAudioTrack } = useRemoteChannel();
  const { getNode, setShowPannel } = useBase();

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
    const isMicPublish = publishedTracks.includes(micTrack);
    const onMicPubClick = () => {
      isMicPublish ? unpublish([micTrack]) : publish([micTrack]);
      setShowPannel(false);
    };

    const onSpeakerSubClick = () => {
      mcuAudioTrack ? unsubscribeMCUAudio() : subscribeMCUAudio();
      setShowPannel(false);
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
        {client?.channelName ? (
          <>
            <Divider style={{ margin: '6px 0' }} />
            <Row className={styles.list}>
              <span>操作</span>
              <div onClick={onMicPubClick}>{isMicPublish ? '取消发布' : '发布'}</div>
              <div onClick={onSpeakerSubClick}>
                {mcuAudioTrack ? '取消订阅全局音频' : '订阅全局音频'}
              </div>
            </Row>
          </>
        ) : null}
      </Row>
    );
  }, [
    micList,
    speakerList,
    micId,
    speakerId,
    micTrack,
    publishedTracks,
    client?.channelName,
    subscribeMCUAudio,
    unsubscribeMCUAudio,
    mcuAudioTrack,
  ]);

  const renderNode = useMemo(() => {
    const node = getNode(DevicePannel, iconType, onClick);
    return node;
  }, [getNode, DevicePannel, iconType, onClick]);

  return renderNode;
});

export const Screen = memo(() => {
  const { operateScreen, screenDimension, screenFrameRate } = useDevice();
  const { publishedTracks, screenTrack, publish, unpublish } = useLocalChannel();
  const { getNode, setShowPannel } = useBase();
  const [showEncoderModal, setShowEncoderModal] = useState(false);
  const setDeviceInfo = useSetRecoilState(deviceInfo);

  const DevicePannel = useMemo(() => {
    const isScreenPublish = publishedTracks.includes(screenTrack);
    const onScreenPubClick = () => {
      if (isScreenPublish) {
        unpublish([screenTrack]);
      } else if (screenTrack) {
        publish([screenTrack]);
      } else if (!screenTrack) {
        operateScreen();
      }
      setShowPannel(false);
    };
    const updateEncoder = (frameRate: number, dimension: VideoDimension) => {
      screenTrack?.setEncoderConfiguration({
        frameRate,
        dimension,
      });
      setDeviceInfo((prev) => ({
        ...prev,
        cameraFrameRate: frameRate,
        cameraDimension: dimension,
      }));
    };
    return (
      <Row>
        <Row className={styles.list}>
          <span>操作</span>
          <div onClick={onScreenPubClick}>{isScreenPublish ? '取消发布' : '发布'}</div>
          <div
            onClick={() => {
              if (isScreenPublish) {
                Toast.info('推流过程中不能调整编码');
                return;
              }
              if (!screenTrack) {
                Toast.info('请先创建共享轨道');
                return;
              }
              setShowEncoderModal(true);
              setShowPannel(false);
            }}
          >
            调整视频编码
          </div>
        </Row>
        <VideoEncoder
          open={showEncoderModal}
          frameRate={screenFrameRate}
          dimension={screenDimension}
          onOk={updateEncoder}
          onClose={() => setShowEncoderModal(false)}
        />
      </Row>
    );
  }, [screenTrack, publishedTracks, screenDimension, screenFrameRate, showEncoderModal]);

  const renderNode = useMemo(() => {
    const node = getNode(DevicePannel, 'iconXDS_share_screen1', operateScreen);
    return node;
  }, [getNode, DevicePannel, operateScreen]);

  return renderNode;
});
