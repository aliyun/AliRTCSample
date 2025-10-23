import { Row, Space, Tooltip, Col, Divider, Typography } from 'ant-design-vue';
import Icon from '../Icon';
import { CheckOutlined } from '@ant-design/icons-vue';
import { useGlobalFlag, useDeviceInfo, useChannelInfo } from '~/store';
import style from './index.module.less';
import { logger } from '~/utils/tools';
import { ref, computed } from 'vue';

const { Text } = Typography;

interface SelectorProps {
  title: string;
  list: MediaDeviceInfo[];
  currentId: string;
  onClick: (deviceId: string) => void;
}

interface DeviceProps {
  click?: () => void;
}

const Selector = (props: SelectorProps) => {
  const { title, list, currentId, onClick } = props;
  if (list.length === 0) return null;
  return (
    <Row class={style.list}>
      <span>{title}</span>
      {list.map((item) => (
        <Col
          key={item.deviceId}
          // @ts-ignore
          onClick={(e) => {
            e.stopPropagation();
            onClick(item.deviceId);
          }}
        >
          {currentId === item.deviceId ? <CheckOutlined class={style.checked} /> : null}
          <Text>{item.label}</Text>
        </Col>
      ))}
    </Row>
  );
};

const useBase = () => {
  const showPanel = ref(false);
  const setShowPannel = (newValue: boolean) => {
    showPanel.value = newValue;
  };
  const getNode = (pannel: any, icon: string, onClick: any, hideArrow = false) => {
    return (
      <Row class={style.wrap}>
        <Space size={[12, 0]}>
          <Icon type={icon} class={style.deviceIcon} onClick={onClick} />
          {hideArrow ? null : (
            <div onClick={(e) => e.stopPropagation()}>
              <Tooltip
                overlayClassName={style.tooltip}
                overlayInnerStyle={{
                  backgroundColor: 'rgba(245,247,250,0.9)',
                }}
                arrow={false}
                onOpenChange={(show) => {
                  showPanel.value = show;
                }}
                open={showPanel.value}
                title={pannel}
                trigger={'click'}
              >
                <Icon
                  class={style.arrow}
                  type={!showPanel.value ? 'icon-XDS_Uparrow' : 'icon-XDS_Downarrow'}
                />
              </Tooltip>
            </div>
          )}
        </Space>
      </Row>
    );
  };
  return {
    getNode,
    showPanel,
    setShowPannel,
  };
};

export const Camera = (props: DeviceProps) => {
  const { click } = props;
  const channelInfo = useChannelInfo();
  const deviceInfo = useDeviceInfo();
  const iconType = computed(() =>
    !deviceInfo.cameraEnable ? 'icon-XDS_FrameMeetingFill' : 'icon-XDS_FrameMeetingLine',
  );
  const { getNode, setShowPannel } = useBase();
  const onCameraClick = (deviceId: string) => {
    deviceInfo.$patch({ cameraId: deviceId });
    logger.info(`camera changeTo ${deviceId}`);
    let newDeviceId = deviceId;
    if (deviceId === '0') {
      newDeviceId = 'environment';
    } else if (deviceId === '1') {
      newDeviceId = 'user';
    }
    channelInfo.cameraTrack?.setDevice(newDeviceId);
    setShowPannel(false);
  };
  const DevicePannel = (
    <Row>
      <Selector
        title="请选择摄像头"
        list={deviceInfo.cameraList}
        currentId={deviceInfo.cameraId}
        onClick={onCameraClick}
      />
    </Row>
  );
  return getNode(DevicePannel, iconType.value, click);
};

export const Mic = (props: DeviceProps) => {
  const { click } = props;
  const globalFlag = useGlobalFlag();
  const channelInfo = useChannelInfo();
  const deviceInfo = useDeviceInfo();
  const { getNode, setShowPannel } = useBase();

  const iconType = computed(() =>
    !deviceInfo.micEnable ? 'icon-XDS_UnMute2Fill' : 'icon-XDS_Mute2',
  );
  const onMicClick = (deviceId: string) => {
    deviceInfo.$patch({ micId: deviceId });
    logger.info(`mic changeTo ${deviceId}`);
    channelInfo.micTrack?.setDevice(deviceId);
    setShowPannel(false);
  };
  const onSpeakerClick = (deviceId: string) => {
    deviceInfo.$patch({ speakerId: deviceId });
    logger.info(`speaker changeTo ${deviceId}`);
    setShowPannel(false);
    channelInfo.mcuAudioTrack?.setSpeaker(deviceId || undefined);
  };
  const DevicePannel = (
    <Row>
      <Selector
        title="请选择麦克风"
        list={deviceInfo.micList}
        currentId={deviceInfo.micId}
        onClick={onMicClick}
      />
      {globalFlag.isMobile ? null : (
        <>
          <Divider style={{ margin: '6px 0' }} />
          <Selector
            title="请选择扬声器"
            list={deviceInfo.speakerList}
            currentId={deviceInfo.speakerId}
            onClick={onSpeakerClick}
          />
        </>
      )}
    </Row>
  );
  return getNode(DevicePannel, iconType.value, click);
};

export const Screen = () => {
  const { getNode } = useBase();
  return getNode(null, 'icon-XDS_share_screen1', null, true);
};
