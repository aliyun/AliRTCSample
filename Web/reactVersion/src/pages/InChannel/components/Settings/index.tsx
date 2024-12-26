import {
  Col,
  Modal,
  Row,
  Typography,
  Menu,
  Divider,
} from 'dingtalk-design-desktop';
import { memo, useCallback, useMemo, useState } from 'react';
import Icon from '~/components/Icon';
import styles from './index.module.less';
import { useLocalChannel } from '~/hooks/channel';
import { LocalTrack } from 'dingrtc';
import General from './General';
import Audio from './Audio';
import Video from './Video';
import ScreenShare from './ScreenShare';
import External from './External';
import Metrics from './Metrics';
import Group from './Group';
  
const { Title, Text } = Typography;
  
  interface ISettingsProps {
    onClose: () => void;
  }
  
const menus = [
  {
    label: '通用',
    icon: <Icon type="iconicon_x_Settings" />,
    key: 'general',
  },
  {
    label: '视频',
    icon: <Icon type="iconXDS_FrameMeetingLine" />,
    key: 'video',
  },
  {
    label: '音频',
    icon: <Icon type="iconXDS_Mute2" />,
    key: 'audio',
  },
  {
    label: '共享屏幕',
    icon: <Icon type="iconXDS_share_screen1" />,
    key: 'share',
  },
  {
    label: '分组讨论',
    icon: <Icon type="iconXDS_Confm" />,
    key: 'group',
  },
  {
    label: '外部源',
    icon: <Icon type="iconXDS_Documents" />,
    key: 'external',
    tip: '仅在当前窗口打开时生效',
  },
  {
    label: '质量监测',
    icon: <Icon type="iconyibiaopan" />,
    key: 'metrics',
  },
];
  
const Settings = (props: ISettingsProps) => {
  const { onClose } = props;
  const [activeTab, setActiveTab] = useState('general');
  const { customAudioTrack, customVideoTrack, publish, unpublish, cameraTrack, micTrack, publishedTracks } = useLocalChannel();
  const [publishStateSnapshot] = useState({ camera: publishedTracks.includes(cameraTrack), mic: publishedTracks.includes(micTrack) })
  const onUnloadExternal = useCallback(async () => {
    const tracksToUnpublish = [customAudioTrack, customVideoTrack].filter((item) =>
      item && publishedTracks.includes(item),
    );
    if (tracksToUnpublish.length) {
      await unpublish(tracksToUnpublish);
    }
    const tracksToPublish: LocalTrack[] = [];
    if (cameraTrack && !publishedTracks.includes(cameraTrack) && publishStateSnapshot.camera) {
      tracksToPublish.push(cameraTrack)
    }
    if (micTrack && !publishedTracks.includes(micTrack) && publishStateSnapshot.mic) {
      tracksToPublish.push(micTrack)
    }
    if (tracksToPublish.length) {
      await publish(tracksToPublish);
    }
  }, [customAudioTrack, customVideoTrack, publish, unpublish, cameraTrack, micTrack, publishedTracks, publishStateSnapshot]);
  const activeMenu = useMemo(() => {
    return menus.find((item) => item.key === activeTab);
  }, [activeTab]);
  
  const onChangeTab = useCallback((key: string) => {
    setActiveTab((prev) => {
      if (prev === 'external') {
        onUnloadExternal();
      }
      return key;
    });
  }, [onUnloadExternal]);
  
  const onCancel = useCallback(() => {
    if (activeTab === 'external') {
      onUnloadExternal();
    }
    onClose();
  }, [onUnloadExternal, onClose, activeTab]);
  
  return (
    <Modal open wrapClassName={styles.settings} width={680} onCancel={onCancel} footer={null}>
      <Row style={{ height: 500 }}>
        <Col span={7} className={styles.settingMenu}>
          <Menu
            onClick={({ key }) => {
              onChangeTab(key);
            }}
            selectedKeys={[activeTab]}
            items={menus}
          />
        </Col>
        <Col className={styles.settingDetail} span={17}>
          <Title level={6}>
            {activeMenu.label}
            {activeMenu.tip ? (
              <Text className={styles.tip} type="secondary">
                  ({activeMenu.tip})
              </Text>
            ) : null}
          </Title>
          <Divider />
          {activeTab === 'general' ? <General /> : null}
          {activeTab === 'audio' ? <Audio /> : null}
          {activeTab === 'video' ? <Video onClose={onClose} /> : null}
          {activeTab === 'share' ? <ScreenShare onClose={onClose} /> : null}
          {activeTab === 'external' ? <External /> : null}
          {activeTab === 'metrics' ? <Metrics /> : null}
          {activeTab === 'group' ? <Group /> : null}
        </Col>
      </Row>
    </Modal>
  );
};
  
export default memo(Settings);
  