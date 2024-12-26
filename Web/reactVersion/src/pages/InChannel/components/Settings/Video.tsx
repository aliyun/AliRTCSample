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
import { memo, useCallback, useEffect, useRef } from 'react';
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
  
const Video = (props: ISettingsProps) => {
  const { onClose } = props;
  const videoRef = useRef<HTMLVideoElement>(null);
  const [{ cameraDimension, cameraFrameRate, cameraId, cameraList }, setDeviceInfo] =
      useRecoilState(deviceInfo);
  const { cameraTrack, publish, unpublish, publishedTracks } = useLocalChannel();
  const [form] = useForm();
  const onFieldsChange = useCallback(
    (changedFields: any[]) => {
      const [{ name, value }] = changedFields;
      if (name[0] === 'cameraId' && cameraTrack) {
        cameraTrack.setDevice(value).then(() => {
          const track = cameraTrack.getMediaStreamTrack();
          videoRef.current.srcObject = new MediaStream([track]);
          videoRef.current.play();
        });
      }
    },
    [cameraTrack],
  );
  
  useEffect(() => {
    if (!cameraTrack) return;
    const track = cameraTrack.getMediaStreamTrack();
    videoRef.current.srcObject = new MediaStream([track]);
    videoRef.current.play();
  }, [cameraTrack]);
  
  const updateEncoder = () => {
    const isCameraPublish = publishedTracks.includes(cameraTrack);
    const { dimension, frameRate, maxBitrate } = form.getFieldsValue();
    const fn = () => {
      if (!cameraTrack) {
        Toast.info('请先创建摄像头轨道');
        return;
      }
      cameraTrack
        .setEncoderConfiguration({
          frameRate,
          dimension,
          maxBitrate,
        })
        .then(() => {
          setDeviceInfo((prev) => ({
            ...prev,
            cameraMaxBitrate: cameraTrack.getMaxBitrate(),
          }));
          publish([cameraTrack]).then(() => {
            Toast.info('调整编码参数并重新发布成功');
            onClose();
          });
        });
    };
    if (isCameraPublish) {
      unpublish([cameraTrack]).then(() => {
        fn();
      });
    } else {
      fn();
    }
    setDeviceInfo((prev) => ({
      ...prev,
      cameraFrameRate: frameRate,
      cameraDimension: dimension,
      cameraMaxBitrate: maxBitrate,
    }));
  };
  
  return (
    <Row className={styles.videoSetting}>
      <div className={styles.videoWrapper}>
        <video loop autoPlay playsInline webkit-playsinline x5-playsinline controls={false} muted ref={videoRef} />
      </div>
      <Form
        form={form}
        onFieldsChange={onFieldsChange}
        style={{ width: '100%' }}
        labelCol={{ span: 4 }}
        labelAlign="left"
        initialValues={{
          cameraId,
          maxBitrate: '',
          dimension: cameraDimension,
          frameRate: cameraFrameRate,
        }}
      >
        <Form.Item label="摄像头" name="cameraId">
          <Select
            options={cameraList.map((item) => ({ label: item.label, value: item.deviceId }))}
          />
        </Form.Item>
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

export default memo(Video);