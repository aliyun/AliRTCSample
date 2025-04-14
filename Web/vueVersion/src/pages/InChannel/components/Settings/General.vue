<template>
  <Row>
    <Form :model="formData" style="width: 100%" :label-col="{ span: 6 }" label-align="left">
      <Form.Item label="发布摄像头流" name="videoPublish" help="开启则发布摄像头流，关闭则取消发布">
        <Checkbox
          v-model:checked="formData.videoPublish"
          :disabled="loading.camera"
          @change="onVideoPublishChange"
        />
      </Form.Item>
      <Form.Item label="发布麦克风流" name="audioPublish" help="开启则发布麦克风流，关闭则取消发布">
        <Checkbox
          v-model:checked="formData.audioPublish"
          :disabled="loading.mic"
          @change="onAudioPublishChange"
        />
      </Form.Item>
      <Form.Item
        label="订阅远端视频"
        name="videoSubscribe"
        help="开启则订阅全部视频，关闭则取消订阅"
      >
        <Checkbox
          v-model:checked="formData.videoSubscribe"
          :disabled="loading.remoteVideo"
          @change="onVideoSubscribeChange"
        />
      </Form.Item>
      <Form.Item label="默认订阅档位" name="defaultRemoteStreamType" help="设置后对下次订阅生效">
        <Select
          v-model:value="formData.defaultRemoteStreamType"
          :options="RemoteStreamTypeOptions"
          @change="onDefaultRemoteStreamType"
        />
      </Form.Item>
      <Form.Item
        label="订阅远端音频"
        name="audioSubscribe"
        help="开启则订阅全部音频，关闭则取消订阅"
      >
        <Checkbox
          v-model:checked="formData.audioSubscribe"
          :disabled="loading.remoteAudio"
          @change="onAudioSubscribeChange"
        />
      </Form.Item>
    </Form>
  </Row>
</template>
<script lang="ts" setup>
import { CameraVideoTrack, MicrophoneAudioTrack } from 'dingrtc';
import { Row, Form, Select, Checkbox } from 'ant-design-vue';
import { computed, reactive, ref } from 'vue';
import { useChannel } from '~/hooks/channel';
import { useClient, useChannelInfo } from '~/store';
import { isIOS } from '~/utils/tools';

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

const RemoteStreamTypeOptions = [
  {
    label: '超清',
    value: 'FHD',
  },
  {
    label: '高清',
    value: 'HD',
  },
  {
    label: '标清',
    value: 'SD',
  },
  {
    label: '流畅',
    value: 'LD',
  },
];

const channelInfo = useChannelInfo();
const client = useClient();
const {
  subscribeMCUAudio,
  unsubscribeMCUAudio,
  subscribeAllRemoteVideo,
  unsubscribeAllRemoteVideo,
  publish,
  unpublish,
} = useChannel();
const loading = ref({
  camera: false,
  mic: false,
  remoteAudio: false,
  remoteVideo: false,
});
const isMicPublish = computed(() =>
  channelInfo.publishedTracks.has(channelInfo.micTrack?.getTrackId()),
);
const isCameraPublish = computed(() =>
  channelInfo.publishedTracks.has(channelInfo.cameraTrack?.getTrackId()),
);

const formData = reactive({
  videoPublish: isCameraPublish.value,
  audioPublish: isMicPublish.value,
  videoSubscribe: channelInfo.subscribeAllVideo,
  defaultRemoteStreamType: channelInfo.defaultRemoteStreamType,
  audioSubscribe: !!channelInfo.mcuAudioTrack,
});


const onVideoPublishChange = (e) => {
  const value = e.target.checked;
  if (value && !isCameraPublish.value) {
    loading.value.camera = true;
    publish([channelInfo.cameraTrack as CameraVideoTrack]).finally(() => {
      loading.value.camera = false;
    });
  }
  if (!value && isCameraPublish.value) {
    loading.value.camera = true;
    unpublish([channelInfo.cameraTrack as CameraVideoTrack]).finally(() => {
      loading.value.camera = false;
    });
  }
};
const onAudioPublishChange = (e) => {
  const value = e.target.checked;
  if (value && !isMicPublish.value) {
    loading.value.mic = true;
    publish([channelInfo.micTrack as MicrophoneAudioTrack]).finally(() => {
      loading.value.mic = false;
    });
  }
  if (!value && isMicPublish.value) {
    loading.value.mic = true;
    unpublish([channelInfo.micTrack as MicrophoneAudioTrack]).finally(() => {
      loading.value.mic = false;
    });
  }
};
const onAudioSubscribeChange = (e) => {
  const value = e.target.checked;
  if (value && !channelInfo.mcuAudioTrack) {
    loading.value.remoteAudio = true;
    subscribeMCUAudio().finally(() => {
      loading.value.remoteAudio = false;
    });
  }
  if (!value && channelInfo.mcuAudioTrack) {
    loading.value.remoteAudio = true;
    unsubscribeMCUAudio().finally(() => {
      loading.value.remoteAudio = false;
    });
  }
};
const onVideoSubscribeChange = (e) => {
  const value = e.target.checked;
  if (value && !channelInfo.subscribeAllVideo) {
    loading.value.remoteVideo = true;

    subscribeAllRemoteVideo().finally(() => {
      loading.value.remoteVideo = false;
    });
  }
  if (!value && channelInfo.subscribeAllVideo) {
    loading.value.remoteVideo = true;
    unsubscribeAllRemoteVideo().finally(() => {
      loading.value.remoteVideo = false;
    });
  }
};
const onDefaultRemoteStreamType = (type) => {
  client.setRemoteDefaultVideoStreamType(type);
};
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
