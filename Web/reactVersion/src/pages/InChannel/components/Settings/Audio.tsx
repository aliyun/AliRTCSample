import {

  Row,

  Divider,
  Form,
  Select,
  Progress,
  Slider,
  Button,
} from 'dingtalk-design-desktop';
import { memo, useCallback, useEffect, useRef, useState } from 'react';
import styles from './index.module.less';
import { useLocalChannel, useRemoteChannel } from '~/hooks/channel';
import { useRecoilState } from 'recoil';
import { deviceInfo } from '~/store';
import DingRTC, { LocalAudioTrack, RemoteAudioTrack } from 'dingrtc';
import testAudio from 'public/1638.mp3';
  

const VolumeWave = (props: { track: LocalAudioTrack | RemoteAudioTrack }) => {
  const interval = useRef<number>(null);
  const { track } = props;
  const [width, setWidth] = useState(0);
  
  useEffect(() => {
    if (!track) return;
    interval.current = window.setInterval(() => {
      setWidth(track.getVolumeLevel() * 100);
    }, 400);
    return () => {
      clearInterval(interval.current);
    };
  }, [track]);
  
  return (
    <div style={{ width: '100%' }}>
      <Progress size="small" percent={width} steps={40} showInfo={false} />
    </div>
  );
};
  
const Audio = () => {
  const { micTrack } = useLocalChannel();
  const { mcuAudioTrack } = useRemoteChannel();
  const [{ micList, micId, speakerId, speakerList }, setDeviceInfo] =
    useRecoilState(deviceInfo);
  const audioRef = useRef<HTMLAudioElement>(null);
  const [testTrack, setTestTrack] = useState<LocalAudioTrack>(null);
  const testDevice = useCallback(() => {
    audioRef.current.onplay = async () => {
      // @ts-ignore
      const captureStream: MediaStream = audioRef.current.captureStream();
      const track = captureStream.getAudioTracks()[0];
      const customTrack = await DingRTC.createCustomAudioTrack({ mediaStreamTrack: track });
      setTestTrack(customTrack);
      setTimeout(() => {
        audioRef.current.pause();
        setTestTrack(null);
      }, 4000);
    };
    if (audioRef.current.setSinkId && audioRef.current.sinkId !== speakerId) {
      audioRef.current.setSinkId(speakerId).then(() => {
        audioRef.current.play();
      });
    } else {
      audioRef.current.play();
    }
  }, [speakerId]);
  const onFieldsChange = useCallback(
    (changedFields: any[]) => {
      const [{ name, value }] = changedFields;
      if (mcuAudioTrack) {
        if (name[0] === 'playVolume') {
          mcuAudioTrack.setVolume(value);
        } else if (name[0] === 'speakerId') {
          mcuAudioTrack.setSpeaker(value);
          setDeviceInfo((prev) => ({ ...prev, speakerId: value }))
        }
      }
      if (name[0] === 'micId' && micTrack) {
        micTrack.setDevice(value);
        setDeviceInfo((prev) => ({ ...prev, micId: value }))
      }
    },
    [mcuAudioTrack, micTrack],
  );
  return (
    <Row>
      <audio ref={audioRef} crossOrigin="anonymous" loop src={testAudio} />
      <Form
        style={{ width: '100%' }}
        labelCol={{ span: 4 }}
        labelAlign="left"
        onFieldsChange={onFieldsChange}
        initialValues={{
          micId,
          playVolume: 1,
          speakerId,
        }}
      >
        <Form.Item label="麦克风" name="micId">
          <Select options={micList.map((item) => ({ label: item.label, value: item.deviceId }))} />
        </Form.Item>
        <Form.Item label="音频能量">
          <VolumeWave track={micTrack} />
        </Form.Item>
        <Divider />
        <Form.Item label="扬声器" className={styles.testDevice}>
          <Form.Item noStyle name="speakerId">
            <Select
              options={speakerList.map((item) => ({ label: item.label, value: item.deviceId }))}
            />
          </Form.Item>
          <Button type="link" onClick={testDevice}>
              测试
          </Button>
        </Form.Item>
        <Form.Item label="音频能量">
          <VolumeWave track={testTrack || mcuAudioTrack} />
        </Form.Item>
        <Form.Item label="播放音量" name="playVolume">
          <Slider max={1} step={0.01} min={0} />
        </Form.Item>
      </Form>
    </Row>
  );
};
  
export default memo(Audio);
  