import {
  Row,
  Form,
  Progress,
  Button,
  Upload,
  UploadFile,
} from 'dingtalk-design-desktop';
import { memo, useCallback, useEffect, useRef, useState } from 'react';
import styles from './index.module.less';
import { useLocalChannel } from '~/hooks/channel';
import DingRTC, { LocalAudioTrack, LocalTrack, RemoteAudioTrack } from 'dingrtc';
import { UploadingOutlined } from 'dd-icons';
import { UploadChangeParam } from 'dingtalk-design-desktop/lib/upload';

type FileChangeFromType = 'av' | 'audio' | 'video';

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

export default memo(External);