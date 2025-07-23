<template>
  <Row class="videoSetting">
    <Form :model="videoConfig" label-align="left">
      <Row :gutter="24">
        <Col :span="8">
          <Form.Item name="width" label="宽度">
            <InputNumber v-model:value="videoConfig.width" placeholder="单位px" />
          </Form.Item>
        </Col>
        <Col :span="8">
          <Form.Item name="height" label="高度">
            <InputNumber v-model:value="videoConfig.height" placeholder="单位px" />
          </Form.Item>
        </Col>
        <Col :span="8">
          <Form.Item name="frameRate" label="帧率">
            <InputNumber v-model:value="videoConfig.frameRate" placeholder="单位fps" />
          </Form.Item>
        </Col>
      </Row>
      <Row justify="space-between">
        <Col>
          <Form.Item label="替换视频">
            <Upload
              :max-count="1"
              :file-list="videoFileList"
              accept=".yuv"
              :before-upload="beforeUpload"
              @change="(changeParam) => onFileChange(changeParam, 'video')"
            >
              <Button> <UploadOutlined />选择yuv视频文件 </Button>
            </Upload>
          </Form.Item>
        </Col>
        <Col>
          <Form.Item>
            <Button @click="onVideoApply">应用视频设置</Button>
          </Form.Item>
        </Col>
      </Row>
    </Form>
    <Divider orientation="left" plain> </Divider>
    <Form :model="audioConfig" label-align="left">
      <Row :gutter="24">
        <Col :span="8">
          <Form.Item name="sampleRate" label="采样率">
            <InputNumber v-model:value="audioConfig.sampleRate" placeholder="单位hz" />
          </Form.Item>
        </Col>
        <Col :span="8">
          <Form.Item name="interval" label="间隔">
            <InputNumber v-model:value="audioConfig.interval" placeholder="单位ms" />
          </Form.Item>
        </Col>
        <Col :span="8">
          <Form.Item name="channels" label="声道">
            <InputNumber v-model:value="audioConfig.channels" />
          </Form.Item>
        </Col>
      </Row>
      <Row justify="space-between">
        <Form.Item label="替换音频">
          <Upload
            :max-count="1"
            :file-list="audioFileList"
            accept=".pcm"
            :before-upload="beforeUpload"
            @change="(changeParam) => onFileChange(changeParam, 'audio')"
          >
            <Button><UploadOutlined />选择pcm音频文件</Button>
          </Upload>
        </Form.Item>
        <Form.Item>
          <Button @click="onAudioApply">应用音频设置</Button>
        </Form.Item>
      </Row>
    </Form>
  </Row>
</template>
<script setup lang="ts">
import {
  Row,
  Form,
  Col,
  Button,
  Upload,
  UploadFile,
  UploadChangeParam,
  InputNumber,
  Divider,
  message,
} from 'ant-design-vue';
import { webUtils } from 'electron';
import { onUnmounted, reactive, ref } from 'vue';
import { useChannel } from '~/hooks/channel';
import { useChannelInfo } from '~/store';
import { loopReadFile } from '~/utils/tools';

import { UploadOutlined } from '@ant-design/icons-vue';
import DingRTC, {
  CameraVideoTrack,
  DingRTCVideoPixelFormat,
  LocalAudioTrack,
  LocalVideoTrack,
  MicrophoneAudioTrack,
} from 'dingrtc-electron-sdk';

type FileChangeFromType = 'audio' | 'video';
const channelInfo = useChannelInfo();
const videoFileList = ref<UploadFile[]>([]);
const audioFileList = ref<UploadFile[]>([]);
const stopVideoPush = ref(null);
const stopAudioPush = ref(null);
const audioFilePath = ref('');
const videoFilePath = ref('');

const videoConfig = reactive<{ width: number; height: number; frameRate: number }>({
  width: undefined,
  height: undefined,
  frameRate: undefined,
});

const audioConfig = reactive<{ channels: number; interval: number; sampleRate: number }>({
  channels: 2,
  sampleRate: undefined,
  interval: undefined,
});

onUnmounted(() => {
  stopAudioPush.value?.();
  stopVideoPush.value?.();
});

const { publish, unpublish } = useChannel();

const beforeUpload = () => false;
const onFileChange = async ({ fileList }: UploadChangeParam<any>, from: FileChangeFromType) => {
  if (!fileList.length) return;
  const filePath = webUtils.getPathForFile(fileList[0].originFileObj);
  if (from === 'audio') {
    audioFilePath.value = filePath;
    audioFileList.value = fileList;
  } else if (from === 'video') {
    videoFileList.value = fileList;
    videoFilePath.value = filePath;
  }
};

const onVideoApply = async () => {
  const { width, height, frameRate } = videoConfig;
  if (!width || !height || !frameRate || !videoFilePath.value) {
    message.error('视频参数未设置');
    return;
  }
  stopVideoPush.value?.();
  const tracksToUnpublish = [
    channelInfo.cameraTrack as CameraVideoTrack,
    channelInfo.customVideoTrack as LocalVideoTrack,
  ].filter((item) => item && channelInfo.publishedTracks.has(item.getTrackId()));
  if (tracksToUnpublish.length) {
    await unpublish(tracksToUnpublish);
  }
  const videoTrack = await DingRTC.createCustomVideoTrack();
  channelInfo.$patch({ customVideoTrack: videoTrack });
  await publish([videoTrack]);
  const interval = Math.floor(1000 / frameRate);
  stopVideoPush.value = loopReadFile(
    { filePath: videoFilePath.value, highWaterMark: width * height * 1.5, interval },
    (data) => {
      videoTrack.pushVideoFrame({
        pixelFormat: DingRTCVideoPixelFormat.I420,
        buffer: data,
        timestamp: Date.now(),
        width,
        height,
        rotation: 0,
        offset: [0, width * height, Math.floor((width * height * 5) / 4)],
        stride: [width, Math.floor(width / 2), Math.floor(width / 2)],
      });
    },
  );
};
const onAudioApply = async () => {
  const { channels, sampleRate, interval } = audioConfig;
  if (!channels || !sampleRate || !interval || !audioFilePath.value) {
    message.error('音频参数未设置');
    return;
  }
  stopAudioPush.value?.();
  const tracksToUnpublish = [
    channelInfo.micTrack as MicrophoneAudioTrack,
    channelInfo.customAudioTrack as LocalAudioTrack,
  ].filter((item) => item && channelInfo.publishedTracks.has(item.getTrackId()));
  if (tracksToUnpublish.length) {
    await unpublish(tracksToUnpublish);
  }
  const audioTrack = await DingRTC.createCustomAudioTrack({
    sampleRate,
    sampleSize: (sampleRate * interval) / 1000,
    stereo: channels === 2,
  });
  channelInfo.$patch({ customAudioTrack: audioTrack });
  await publish([audioTrack]);
  const highWaterMark = ((sampleRate * interval) / 1000) * channels * 2;
  stopAudioPush.value = loopReadFile(
    { filePath: audioFilePath.value, highWaterMark, interval },
    (data) => {
      audioTrack.pushAudioFrame({
        buffer: data,
        channels,
        sampleRate,
        timestamp: Date.now(),
        samples: (sampleRate * interval) / 1000,
      });
    },
  );
};
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
