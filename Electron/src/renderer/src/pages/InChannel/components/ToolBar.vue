<template>
  <Row :class="['toolBarWrap', globalFlag.immersive ? 'hideBar' : '']">
    <Row class="confInfo" :gutter="16">
      <Col>
        <span>应用: </span>
        <span>{{ currentUserInfo.appId }}</span>
      </Col>
      <Col>
        <span>频道: </span>
        <span>{{ currentUserInfo.channel }}</span>
      </Col>
    </Row>
    <Space :size="[0, 8]">
      <Row class="toolBtn" @click="operateCamera">
        <Camera />
        <span>{{ deviceInfo.cameraEnable ? '关摄像头' : '开摄像头' }}</span>
      </Row>
      <Row class="toolBtn" @click="operateMic">
        <Mic />
        <span>{{ deviceInfo.micEnable ? '静音' : '解除静音' }}</span>
      </Row>
      <Row
        :class="['toolBtn', { stopShare: channelInfo.screenTrack }]"
        @click="toggleShowShareSelector"
      >
        <Screen />
        <span>{{ channelInfo.screenTrack ? '结束共享' : '共享' }}</span>
      </Row>
      <Row class="toolBtn" @click="toggleShowSettings">
        <Icon type="icon-icon_x_Settings" />
        <span style="margin-top: 4px">设置</span>
      </Row>
    </Space>
    <Button type="primary" danger @click="onLeave">
      离开
    </Button>
    <Settings v-if="showSetting" :close="toggleShowSettings" />
    <ShareSelector v-if="showShareSelector" :close="toggleShowShareSelector" />
  </Row>
</template>
<script lang="ts" setup>
import { Button, Row, Space, Col } from 'ant-design-vue';
import {
  useClient,
  useCurrentUserInfo,
  useGlobalFlag,
  useChannelInfo,
  useDeviceInfo,
} from '~/store';
import { Camera, Mic, Screen } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import Icon from '~/components/Icon';
import ShareSelector from './ShareSelector.vue';
import Settings from './Settings/index.vue';
import { onMounted, onUnmounted, ref, toRaw } from 'vue';

interface IProps {
  onLeave: () => void;
}
const props = defineProps<IProps>();
const channelInfo = useChannelInfo();
const deviceInfo = useDeviceInfo();
const globalFlag = useGlobalFlag();
const client = useClient();
const currentUserInfo = useCurrentUserInfo();
const showSetting = ref(false);
const showShareSelector = ref(false);
const { operateCamera, operateMic, operateScreen } = useDevice('in');

const onLeave = async () => {
  toRaw(client).leave();
  props.onLeave();
};

const timer = ref(0);

const hideAfterStill = () => {
  if (timer.value) clearTimeout(timer.value);
  globalFlag.$patch({ immersive: false });
  timer.value = window.setTimeout(() => {
    globalFlag.$patch({ immersive: true });
  }, 10000);
};

onMounted(() => {
  document.addEventListener('mousemove', hideAfterStill);
});

onUnmounted(() => {
  document.removeEventListener('mousemove', hideAfterStill);
});

const toggleShowShareSelector = () => {
  if (!channelInfo.screenTrack) {
    showShareSelector.value = !showShareSelector.value
  } else {
    if (deviceInfo.shareSystemAudio) {
      deviceInfo.$patch({ shareSystemAudio: false });
      channelInfo.micTrack?.enableSystemAudio(false);
    }
    if (channelInfo.publishedTracks.has(channelInfo.screenTrack.getTrackId())) {
      operateScreen()
    }
  }
}

const toggleShowSettings = () => {
  showSetting.value = !showSetting.value;
};
</script>
<style lang="less" scoped>
@import url('../index.module.less');
</style>
