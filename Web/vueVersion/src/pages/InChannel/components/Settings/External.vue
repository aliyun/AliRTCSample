<template>
  <Row class="videoSetting">
    <div class="videoWrapper">
      <video ref="videoRef" loop autoPlay playsInline webkit-playsinline x5-playsinline :controls="false" />
      <audio ref="audioRef" loop autoPlay :controls="false" />
    </div>
    <Form style="width: 100%" :label-col="{ span: 5 }" label-align="left">
      <Form.Item label="音频能量">
        <VolumeLevelBar :track="channelInfo.customAudioTrack" />
      </Form.Item>
      <Form.Item label="替换音视频">
        <Upload
          :max-count="1"
          :file-list="avFileList"
          accept="video/*"
          :before-upload="beforeUpload"
          @change="(changeParam) => onFileChange(changeParam, 'av')"
        >
          <Button>
            <UploadOutlined />选择视频文件
          </Button>
        </Upload>
      </Form.Item>
      <Form.Item label="替换视频">
        <Upload
          :max-count="1"
          :file-list="videoFileList"
          accept="video/*"
          :before-upload="beforeUpload"
          @change="(changeParam) => onFileChange(changeParam, 'video')"
        >
          <Button>
            <UploadOutlined />选择视频文件
          </Button>
        </Upload>
      </Form.Item>
      <Form.Item label="替换音频">
        <Upload
          :max-count="1"
          :file-list="audioFileList"
          accept="audio/mpeg, audio/wav"
          :before-upload="beforeUpload"
          @change="(changeParam) => onFileChange(changeParam, 'audio')"
        >
          <Button><UploadOutlined />选择音频文件</Button>
        </Upload>
      </Form.Item>
    </Form>
  </Row>
</template>
<script setup lang="ts">
import {
  Row,
  Form,
  Button,
  Upload,
  UploadFile,
  UploadChangeParam,
} from 'ant-design-vue';
import { useChannel } from '~/hooks/channel';
import DingRTC, { CameraVideoTrack, LocalAudioTrack, LocalTrack, LocalVideoTrack, MicrophoneAudioTrack } from 'dingrtc';
import { UploadOutlined } from '@ant-design/icons-vue';
import VolumeLevelBar from './VolumeLevelBar.vue';
import { onMounted, ref } from 'vue';
import { useChannelInfo } from '~/store';

type FileChangeFromType = 'av' | 'audio' | 'video';


const channelInfo = useChannelInfo();
const videoRef = ref<HTMLVideoElement>(null);
const audioRef = ref<HTMLAudioElement>(null);
const avFileList = ref<UploadFile[]>([]);
const videoFileList = ref<UploadFile[]>([]);
const audioFileList = ref<UploadFile[]>([]);
const enableAudioRef = ref(false);
const { publish, unpublish } = useChannel();
onMounted(() => {
  videoRef.value.onloadedmetadata = async () => {
    const stream = videoRef.value.captureStream();
    const videoTrack = await DingRTC.createCustomVideoTrack({
      mediaStreamTrack: stream.getVideoTracks()[0],
    });
    channelInfo.$patch({ customVideoTrack: videoTrack });
    const tracksToPublish: LocalTrack[] = [videoTrack];
    if (enableAudioRef.value && stream.getAudioTracks().length) {
      const audioTrack = await DingRTC.createCustomAudioTrack({
        mediaStreamTrack: stream.getAudioTracks()[0],
      });
      channelInfo.$patch({ customAudioTrack: audioTrack });
      tracksToPublish.push(audioTrack);
    }
    await publish(tracksToPublish);
  };
  audioRef.value.onloadedmetadata = async () => {
    const stream = audioRef.value.captureStream();
    const audioTrack = await DingRTC.createCustomAudioTrack({
      mediaStreamTrack: stream.getAudioTracks()[0],
    });
    channelInfo.$patch({ customAudioTrack: audioTrack });
    await publish([audioTrack]);
  };
});
const beforeUpload = () => false;
const onFileChange = async ({ file, fileList }: UploadChangeParam<any>, from: FileChangeFromType) => {
  let tracksToUnpublish: LocalTrack[] = [];
  if (from === 'av') {
    avFileList.value = fileList;
    tracksToUnpublish = [
      channelInfo.cameraTrack as CameraVideoTrack,
      channelInfo.micTrack as MicrophoneAudioTrack,
      channelInfo.customAudioTrack as LocalAudioTrack,
      channelInfo.customVideoTrack as LocalVideoTrack,
    ].filter((item) => item && channelInfo.publishedTracks.has(item.getTrackId()));
  } else if (from === 'audio') {
    audioFileList.value = fileList;
    tracksToUnpublish = [channelInfo.micTrack as MicrophoneAudioTrack, channelInfo.customAudioTrack as LocalAudioTrack].filter((item) =>
      item && channelInfo.publishedTracks.has(item.getTrackId()),
    );
  } else if (from === 'video') {
    videoFileList.value = fileList;
    tracksToUnpublish = [channelInfo.cameraTrack as CameraVideoTrack, channelInfo.customVideoTrack as LocalVideoTrack].filter((item) =>
      item && channelInfo.publishedTracks.has(item.getTrackId()),
    );
  }
  if (tracksToUnpublish.length) {
    await unpublish(tracksToUnpublish);
  }
  const url = URL.createObjectURL(file);
  enableAudioRef.value = from === 'av';
  videoRef.value.muted = !enableAudioRef.value;
  if (from === 'av') {
    audioRef.value.pause();
  }
  if (from !== 'audio') {
    videoRef.value.src = url;
    videoRef.value.play();
  } else {
    audioRef.value.src = url;
    audioRef.value.play();
  }
};

</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>