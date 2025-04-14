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
      <Row class="toolBtn" @click="onClickCamera">
        <Camera />
        <span>{{ deviceInfo.cameraEnable ? '关摄像头' : '开摄像头' }}</span>
      </Row>
      <Row class="toolBtn" @click="operateMic">
        <Mic />
        <span>{{ deviceInfo.micEnable ? '静音' : '解除静音' }}</span>
      </Row>
      <Row
        v-if="!globalFlag.isMobile"
        :class="['toolBtn', { stopShare: channelInfo.screenTrack }]"
        @click="operateScreen"
      >
        <Screen />
        <span>{{ channelInfo.screenTrack ? '结束共享' : '共享' }}</span>
      </Row>
      <Row
        v-if="!globalFlag.isMobile"
        class="toolBtn"
        @click="toggleRTM"
      >
        <Icon type="icon-icon_XDS_FrameMessage" />
        <span style="margin-top: 4px">聊天</span>
      </Row>
      <Row class="toolBtn" @click="toggleWhiteboard">
        <Icon type="icon-whiteboard1" />
        <span style="margin-top: 4px">{{
          channelInfo.isWhiteboardOpen ? '关闭白板' : '白板'
        }}</span>
      </Row>
      <Row class="toolBtn" @click="toggleShowSettings">
        <Icon type="icon-icon_x_Settings" />
        <span style="margin-top: 4px">设置</span>
      </Row>
    </Space>
    <Button type="primary" danger @click="onLeave"> 离开 </Button>
    <Settings v-if="showSetting" :close="toggleShowSettings" />
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
  useRTMInfo,
} from '~/store';
import { Camera, Mic, Screen } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import Icon from '~/components/Icon';
import Settings from './Settings/index.vue';
import { ref } from 'vue';
import { useWhiteboardHooks } from '~/hooks/channel';
import { DEFAULT_WHITEBOARD_ID } from '~/constants';

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
const { openWhiteboard, closeWhiteboard } = useWhiteboardHooks();
const { operateCamera, operateMic, operateScreen } = useDevice('in');
const rtmInfo = useRTMInfo();

const onClickCamera = () => {
  const noCamera = !channelInfo.cameraTrack;
  if (globalFlag.isMobile) {
    operateCamera().then(() => {
      if (!noCamera) return;
      channelInfo.updateTrackStats(currentUserInfo.userId);
      if (!channelInfo.mainViewUserId) {
        channelInfo.mainViewUserId = currentUserInfo.userId;
        channelInfo.mainViewPreferType = 'camera';
      }
    });
  } else {
    operateCamera();
  }
};

const onLeave = async () => {
  client.leave();
  props.onLeave();
};

const toggleShowSettings = () => {
  showSetting.value = !showSetting.value;
};

const toggleRTM = () => {
  rtmInfo.enabled = !rtmInfo.enabled
}

const toggleWhiteboard = () => {
  if (channelInfo.isWhiteboardOpen) {
    closeWhiteboard(DEFAULT_WHITEBOARD_ID);
  } else {
    openWhiteboard(DEFAULT_WHITEBOARD_ID);
  }
};
</script>
<style lang="less" scoped>
@import url('../index.module.less');
</style>
