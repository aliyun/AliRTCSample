import {
  Col,
  Modal,
  Row,
  Typography,
  Menu,
  Divider,
  Form,
  Select,
  InputNumber,
  Progress,
  Slider,
  Button,
  Toast,
  Upload,
  UploadFile,
  Checkbox,
} from 'dingtalk-design-desktop';
import { ReactNode, memo, useCallback, useEffect, useMemo, useRef, useState } from 'react';
import Icon from '~/components/Icon';
import styles from '../index.module.less';
import { useLocalChannel, useNetworkStats, useRemoteChannel } from '~/hooks/channel';
import { useRecoilState } from 'recoil';
import { deviceInfo } from '~/store';
import DingRTC, { LocalAudioTrack, LocalTrack, RemoteAudioTrack } from 'dingrtc';
import { useForm } from 'dingtalk-design-desktop/lib/form/Form';
import testAudio from 'public/1638.mp3';
import { UploadingOutlined } from 'dd-icons';
import { UploadChangeParam } from 'dingtalk-design-desktop/lib/upload';

const { Title, Text } = Typography;
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

const frameRates = [5, 10, 15, 20, 30];

interface ISettingsProps {
  onClose: () => void;
}

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

const Statistic = (props: { title: string, items: Array<{ label: string, value: ReactNode }> }) => {
  const { title, items } = props;
  return (
    <Row className={styles.qualityItem}>
      <Row>{title}</Row>
      <Row>
        {items.map((item, idx) => (
          <Col span={8} key={`${item.label}${idx}`}>
            <Row className={styles.qualityLabel}>
              <Col span={8} >{item.label}</Col>
              <Col span={14}>{item.value}</Col>
            </Row>
          </Col>
        ))}
      </Row>
    </Row>
  )
}



const Quality = memo(() => {
  const { rtcStats } = useNetworkStats();

  const networkItems = [
    {
      label: '带宽',
      value: <span>&uarr; {rtcStats.localBitrate ? `${Math.round(rtcStats.localBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '丢包率',
      value: `${rtcStats.loss} %`,
    },
    {
      label: '延迟',
      value: rtcStats.rtt ? `${rtcStats.rtt} ms` : '--',
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteBitrate ? `${Math.round(rtcStats.remoteBitrate / 1000)} kbps` : '--'}</span>,
    },

  ]
  const audioItems = [
    {
      label: '码率',
      value: <span>&uarr; {rtcStats.localAudioBitrate ? `${Math.round(rtcStats.localAudioBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteAudioBitrate ? `${Math.round(rtcStats.remoteAudioBitrate / 1000)} kbps` : '--'}</span>,
    },
  ]
  const videoItems = [
    {
      label: '分辨率',
      value: <span>&uarr; {rtcStats.localCameraResolution ? `${rtcStats.localCameraResolution.width}*${rtcStats.localCameraResolution.height}` : '--'}</span>,
    },
    {
      label: '码率',
      value: <span>&uarr; {rtcStats.localCameraBitrate ? `${Math.round(rtcStats.localCameraBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '帧率',
      value: <span>&uarr; {rtcStats.localCameraFPS ? `${rtcStats.localCameraFPS} fps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteCameraResolution ? `${rtcStats.remoteCameraResolution.width}*${rtcStats.remoteCameraResolution.height}` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteCameraBitrate ? `${Math.round(rtcStats.remoteCameraBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteCameraFPS ? `${rtcStats.remoteCameraFPS} fps` : '--'}</span>,
    },
  ]
  const screenItems = [
    {
      label: '分辨率',
      value: <span>&uarr; {rtcStats.localScreenResolution ? `${rtcStats.localScreenResolution.width}*${rtcStats.localScreenResolution.height}` : '--'}</span>,
    },
    {
      label: '码率',
      value: <span>&uarr; {rtcStats.localScreenBitrate ? `${Math.round(rtcStats.localScreenBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '帧率',
      value: <span>&uarr; {rtcStats.localScreenFPS ? `${rtcStats.localScreenFPS} fps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteScreenResolution ? `${rtcStats.remoteScreenResolution.width}*${rtcStats.remoteScreenResolution.height}` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteScreenBitrate ? `${Math.round(rtcStats.remoteScreenBitrate / 1000)} kbps` : '--'}</span>,
    },
    {
      label: '',
      value: <span>&darr; {rtcStats.remoteScreenFPS ? `${rtcStats.remoteScreenFPS} fps` : '--'}</span>,
    },
  ]

  return (
    <Row>
      <Statistic title='网络' items={networkItems} />
      <Statistic title='音频' items={audioItems} />
      <Statistic title='视频' items={videoItems} />
      <Statistic title='屏幕分享' items={screenItems} />
    </Row>
  )
});

const General = () => {
  const { micTrack, publishedTracks, cameraTrack, publish, unpublish } = useLocalChannel();
  const { mcuAudioTrack, subscribeMCUAudio, unsubscribeMCUAudio, subscribeAllVideo, subscribeAllRemoteVideo, unsubscribeAllRemoteVideo } = useRemoteChannel();
  const [loading, setLoading] = useState({
    camera: false,
    mic: false,
    remoteAudio: false,
    remoteVideo: false,
  })
  const isMicPublish = useMemo(() => publishedTracks.includes(micTrack), [micTrack, publishedTracks]);
  const isCameraPublish = useMemo(() => publishedTracks.includes(cameraTrack), [cameraTrack, publishedTracks]);
  const onFieldsChange = useCallback(
    (changedFields: any[]) => {
      const [{ name, value }] = changedFields;
      if (name[0] === 'videoPublish') {
        if (value && !isCameraPublish) {
          setLoading((prev) => ({ ...prev, camera: true }))
          publish([cameraTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, camera: false }))
          });
        }
        if (!value && isCameraPublish) {
          setLoading((prev) => ({ ...prev, camera: true }))
          unpublish([cameraTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, camera: false }))
          });
        }
      } else if (name[0] === 'audioPublish') {
        if (value && !isMicPublish) {
          setLoading((prev) => ({ ...prev, mic: true }))
          publish([micTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, mic: false }))
          });;
        }
        if (!value && isMicPublish) {
          setLoading((prev) => ({ ...prev, mic: true }))
          unpublish([micTrack]).finally(() => {
            setLoading((prev) => ({ ...prev, mic: false }))
          });
        }
      } else if (name[0] === 'videoSubscribe') {
        if (value && !subscribeAllVideo) {
          setLoading((prev) => ({ ...prev, remoteVideo: true }))
          subscribeAllRemoteVideo().finally(() => {
            setLoading((prev) => ({ ...prev, remoteVideo: false }))
          });
        }
        if (!value && subscribeAllVideo) {
          setLoading((prev) => ({ ...prev, remoteVideo: true }))
          unsubscribeAllRemoteVideo().finally(() => {
            setLoading((prev) => ({ ...prev, remoteVideo: false }))
          });
        }
      } else if (name[0] === 'audioSubscribe') {
        if (value && !mcuAudioTrack) {
          setLoading((prev) => ({ ...prev, remoteAudio: true }))
          subscribeMCUAudio().finally(() => {
            setLoading((prev) => ({ ...prev, remoteAudio: false }))
          });
        }
        if (!value && mcuAudioTrack) {
          setLoading((prev) => ({ ...prev, remoteAudio: true }))
          unsubscribeMCUAudio().finally(() => {
            setLoading((prev) => ({ ...prev, remoteAudio: false }))
          });
        }
      }
    },
    [mcuAudioTrack, isMicPublish, isCameraPublish, mcuAudioTrack, subscribeAllVideo, subscribeAllRemoteVideo, unsubscribeAllRemoteVideo],
  );
  return (
    <Row>
      <Form
        style={{ width: '100%' }}
        labelCol={{ span: 6 }}
        labelAlign="left"
        onFieldsChange={onFieldsChange}
        initialValues={{
          videoPublish: isCameraPublish,
          audioPublish: isMicPublish,
          videoSubscribe: subscribeAllVideo,
          audioSubscribe: !!mcuAudioTrack,
        }}
      >
        <Form.Item label="发布摄像头流" name="videoPublish" valuePropName='checked' help="开启则发布摄像头流，关闭则取消发布">
          <Checkbox disabled={loading.camera} />
        </Form.Item>
        <Form.Item label="发布麦克风流" name="audioPublish" valuePropName='checked' help="开启则发布麦克风流，关闭则取消发布">
          <Checkbox disabled={loading.mic} />
        </Form.Item>
        <Form.Item label="订阅远端视频" name="videoSubscribe" valuePropName='checked' help="开启则订阅全部视频，关闭则取消订阅">
          <Checkbox disabled={loading.remoteVideo} />
        </Form.Item>
        <Form.Item label="订阅远端音频" name="audioSubscribe" valuePropName='checked' help="开启则订阅全部音频，关闭则取消订阅">
          <Checkbox disabled={loading.remoteAudio} />
        </Form.Item>
      </Form>
    </Row>
  )
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

type FileChangeFromType = 'av' | 'audio' | 'video';

const External = () => {
  const videoRef = useRef<HTMLVideoElement>(null);
  const audioRef = useRef<HTMLAudioElement>(null);
  const [avFileList, setAvFileList] = useState<UploadFile[]>([]);
  const [videoFileList, setVideoFileList] = useState<UploadFile[]>([]);
  const [audioFileList, setAudioFileList] = useState<UploadFile[]>([]);
  const enableAudioRef = useRef(false);
  const {
    cameraTrack,
    publish,
    unpublish,
    micTrack,
    publishedTracks,
    customAudioTrack,
    customVideoTrack,
    setLocalChannelInfo,
  } = useLocalChannel();
  useEffect(() => {
    videoRef.current.onloadedmetadata = async () => {
      // @ts-ignore
      const stream = videoRef.current.captureStream();
      const videoTrack = await DingRTC.createCustomVideoTrack({
        mediaStreamTrack: stream.getVideoTracks()[0],
      });
      setLocalChannelInfo((prev) => ({ ...prev, customVideoTrack: videoTrack }));
      const tracksToPublish: LocalTrack[] = [videoTrack];
      if (enableAudioRef.current && stream.getAudioTracks().length) {
        const audioTrack = await DingRTC.createCustomAudioTrack({
          mediaStreamTrack: stream.getAudioTracks()[0],
        });
        setLocalChannelInfo((prev) => ({ ...prev, customAudioTrack: audioTrack }));
        tracksToPublish.push(audioTrack);
      }
      await publish(tracksToPublish);
    };
    audioRef.current.onloadedmetadata = async () => {
      // @ts-ignore
      const stream = audioRef.current.captureStream();
      const audioTrack = await DingRTC.createCustomAudioTrack({
        mediaStreamTrack: stream.getAudioTracks()[0],
      });
      setLocalChannelInfo((prev) => ({ ...prev, customAudioTrack: audioTrack }));
      await publish([audioTrack]);
    };
  }, [cameraTrack, micTrack, publish, unpublish, customAudioTrack, customVideoTrack]);
  const onFileChange = useCallback(
    async ({ file, fileList }: UploadChangeParam<any>, from: FileChangeFromType) => {
      let tracksToUnpublish: LocalTrack[] = [];
      if (from === 'av') {
        setAvFileList(fileList);
        tracksToUnpublish = [
          cameraTrack,
          micTrack,
          customAudioTrack,
          customVideoTrack,
        ].filter((item) => item && publishedTracks.includes(item));
      } else if (from === 'audio') {
        setAudioFileList(fileList);
        tracksToUnpublish = [micTrack, customAudioTrack].filter((item) =>
          item && publishedTracks.includes(item),
        );
      } else if (from === 'video') {
        setVideoFileList(fileList);
        tracksToUnpublish = [cameraTrack, customVideoTrack].filter((item) =>
          item && publishedTracks.includes(item),
        );
      }
      if (tracksToUnpublish.length) {
        await unpublish(tracksToUnpublish);
      }
      const url = URL.createObjectURL(file);
      enableAudioRef.current = from === 'av';
      videoRef.current.muted = !enableAudioRef.current;
      if (from === 'av') {
        audioRef.current.pause();
      }
      if (from !== 'audio') {
        videoRef.current.src = url;
        videoRef.current.play();
      } else {
        audioRef.current.src = url;
        audioRef.current.play();
      }
    },
    [
      cameraTrack,
      publish,
      unpublish,
      customVideoTrack,
      customAudioTrack,
      micTrack,
      publishedTracks,
    ],
  );

  return (
    <Row className={styles.videoSetting}>
      <div className={styles.videoWrapper}>
        <video loop autoPlay playsInline webkit-playsinline x5-playsinline controls={false} ref={videoRef} />
        <audio loop autoPlay controls={false} ref={audioRef} />
      </div>
      <Form style={{ width: '100%' }} labelCol={{ span: 5 }} labelAlign="left">
        <Form.Item label="音频能量">
          <VolumeWave track={customAudioTrack} />
        </Form.Item>
        <Form.Item label="替换音视频">
          <Upload
            maxCount={1}
            fileList={avFileList}
            accept="video/*"
            onChange={(changeParam) => onFileChange(changeParam, 'av')}
            beforeUpload={() => false}
            itemRender={() => null}
          >
            <Button icon={<UploadingOutlined />}>选择视频文件</Button>
          </Upload>
        </Form.Item>
        <Form.Item label="替换视频">
          <Upload
            maxCount={1}
            accept="video/*"
            fileList={videoFileList}
            onChange={(changeParam) => onFileChange(changeParam, 'video')}
            beforeUpload={() => false}
            itemRender={() => null}
          >
            <Button icon={<UploadingOutlined />}>选择视频文件</Button>
          </Upload>
        </Form.Item>
        <Form.Item label="替换音频">
          <Upload
            maxCount={1}
            fileList={audioFileList}
            accept="audio/mpeg, audio/wav"
            onChange={(changeParam) => onFileChange(changeParam, 'audio')}
            beforeUpload={() => false}
            itemRender={() => null}
          >
            <Button icon={<UploadingOutlined />}>选择音频文件</Button>
          </Upload>
        </Form.Item>
      </Form>
    </Row>
  );
};

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
    label: '外部源',
    icon: <Icon type="iconXDS_Documents" />,
    key: 'external',
    tip: '仅在当前窗口打开时生效',
  },
  {
    label: '质量监测',
    icon: <Icon type="iconyibiaopan" />,
    key: 'quality',
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
          {activeTab === 'share' ? <Share onClose={onClose} /> : null}
          {activeTab === 'external' ? <External /> : null}
          {activeTab === 'quality' ? <Quality /> : null}
        </Col>
      </Row>
    </Modal>
  );
};

export default memo(Settings);
