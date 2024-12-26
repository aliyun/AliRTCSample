import {
  Row,
  Typography,
  Divider,
  Form,
  Select,
  InputNumber,
  Button,
  Toast,
} from 'dingtalk-design-desktop';
import { memo } from 'react';
import styles from './index.module.less';
import { useLocalChannel } from '~/hooks/channel';
import { useRecoilState } from 'recoil';
import { deviceInfo } from '~/store';
import { useForm } from 'dingtalk-design-desktop/lib/form/Form';
import { isIOS } from '~/utils/tools';
  
  
const { Text } = Typography;
const videoDimensions = [
  'VD_120x120',
  'VD_160x120',
  'VD_180x180',
  'VD_240x180',
  'VD_320x180',
  'VD_240x240',
  'VD_320x240',
  'VD_424x240',
  'VD_360x360',
  'VD_480x360',
  'VD_640x360',
  'VD_480x480',
  'VD_640x480',
  'VD_840x480',
  'VD_960x540',
  'VD_960x720',
  'VD_1280x720',
  'VD_1920x1080',
];
  
if (isIOS()) {
  videoDimensions.pop();
}
  
const frameRates = [5, 10, 15, 20, 30];
  
  interface ISettingsProps {
    onClose: () => void;
  }
  
const Share = (props: ISettingsProps) => {
  const { onClose } = props;
  const [{ screenDimension, screenFrameRate }, setDeviceInfo] = useRecoilState(deviceInfo);
  const { screenTrack, publish, unpublish, publishedTracks } = useLocalChannel();
  
  const [form] = useForm();
  
  const updateEncoder = () => {
    const { dimension, frameRate, maxBitrate } = form.getFieldsValue();
    const isScreenPublish = publishedTracks.includes(screenTrack);
  
    const fn = () => {
      if (!screenTrack) {
        Toast.info('请先创建共享轨道');
        return;
      }
      screenTrack
        .setEncoderConfiguration({
          frameRate,
          dimension,
          maxBitrate,
        })
        .then(() => {
          setDeviceInfo((prev) => ({
            ...prev,
            screenMaxBitrate: screenTrack.getMaxBitrate(),
          }));
          publish([screenTrack]).then(() => {
            Toast.info('调整编码参数并重新发布成功');
            onClose();
          });
        });
    };
    if (isScreenPublish) {
      unpublish([screenTrack]).then(() => {
        fn();
      });
    } else {
      fn();
    }
    setDeviceInfo((prev) => ({
      ...prev,
      screenDimension: dimension,
      screenMaxBitrate: maxBitrate,
      screenFrameRate: frameRate,
    }));
  };
  return (
    <Row>
      <Form
        form={form}
        style={{ width: '100%' }}
        labelCol={{ span: 4 }}
        labelAlign="left"
        initialValues={{
          dimension: screenDimension,
          frameRate: screenFrameRate,
          maxBitrate: '',
        }}
      >
        <Divider orientation="left" plain>
          <Text>视频编码设置</Text>
        </Divider>
        <Form.Item label="分辨率" name="dimension">
          <Select options={videoDimensions.map((item) => ({ label: item, value: item }))} />
        </Form.Item>
        <Form.Item label="帧率" name="frameRate">
          <Select options={frameRates.map((item) => ({ label: item, value: item }))} />
        </Form.Item>
        <Form.Item label="最大码率" name="maxBitrate">
          <InputNumber placeholder="默认用分辨率、帧率计算" addonAfter={'Kbps'} />
        </Form.Item>
        <Button className={styles.apply} onClick={updateEncoder}>
            应用
        </Button>
      </Form>
    </Row>
  );
};
  
export default memo(Share);
  