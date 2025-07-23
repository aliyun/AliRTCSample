<script lang="ts" setup>
import { ref, onMounted, onUnmounted, computed, watch, watchEffect } from 'vue';
import SmallView from './components/SmallView.vue';
import MainView from './components/MainView.vue';
import ToolBar from './components/ToolBar.vue';
import { NetworkDetector } from './components/NetworkBar';
import Icon from '~/components/Icon';
import { useClient, useGlobalFlag, useChannelInfo } from '~/store';

import { useInChannelHooks } from '~/hooks/channel';
import { parseTime } from '~/utils/tools';
import { Divider, Col, Radio, RadioGroup, Row, Space, Tooltip } from 'ant-design-vue';
import { LocalVideoTrack } from 'dingrtc-electron-sdk';

const wrapRef = ref(null);
const timeLeftTimer = ref(0);
const fullscreen = ref(false);
const channelInfo = useChannelInfo();
const client = useClient();
const globalFlag = useGlobalFlag();
const timeLeft = ref(channelInfo.timeLeft - 1);
const gridHeight = ref(0);
const immersiveTimer = ref(0);
const { clearRoom } = useInChannelHooks();

const hideAfterStill = () => {
  if (immersiveTimer.value) clearTimeout(immersiveTimer.value);
  globalFlag.$patch({ immersive: false });
  immersiveTimer.value = window.setTimeout(() => {
    globalFlag.$patch({ immersive: true });
  }, 10000);
};

const onFullScreen = () => {
  if (!document.fullscreenElement) {
    fullscreen.value = true;
    document.documentElement.requestFullscreen();
  } else if (document.exitFullscreen) {
    fullscreen.value = false;
    document.exitFullscreen();
  }
};

const onExitFullScreen = () => {
  if (!document.fullscreenElement) fullscreen.value = false;
};

const clearTrack = () => {
  channelInfo.screenTrack.stop();
  client.unpublish([channelInfo.screenTrack as LocalVideoTrack]);
  channelInfo.$patch({ screenTrack: null });
};

onMounted(() => {
  document.addEventListener('fullscreenchange', onExitFullScreen);
  timeLeftTimer.value = window.setInterval(() => {
    timeLeft.value = Math.max(timeLeft.value - 1, 0);
  }, 1000);

  document.addEventListener('mousemove', hideAfterStill);
});

watch(() => channelInfo.screenTrack, () => {
  channelInfo.updateTrackStats();
}, {
  immediate: true
})

watchEffect((cleanUp) => {
  if (!channelInfo.screenTrack) return;
  channelInfo.screenTrack?.on('track-ended', clearTrack);
  cleanUp(() => {
    channelInfo.screenTrack?.off('track-ended', clearTrack);
  });
});

onUnmounted(() => {
  clearInterval(timeLeftTimer.value);
  clearTimeout(immersiveTimer.value);
  document.removeEventListener('mousemove', hideAfterStill);
  document.removeEventListener('fullscreenchange', onExitFullScreen);
});

const spanNum = computed(() => {
  const userCount = channelInfo.allUsers.length;
  if (userCount > 9) return 6;
  else if (userCount > 4) return 8;
  else if (userCount > 1) return 12;
  return 24;
});

watch(
  () => [wrapRef.value, channelInfo.allUsers],
  (newValue) => {
    let clientHeight = newValue[0].$el.clientHeight - 12;
    const userCount = newValue[1].length;
    if (userCount > 9) clientHeight = clientHeight / 4;
    else if (userCount > 4) clientHeight = clientHeight / 3;
    else if (userCount > 1) clientHeight = clientHeight / 2;
    gridHeight.value = clientHeight;
  },
);
</script>

<template>
  <Row class="blockWrapper" ref="wrapRef">
    <Row :class="globalFlag.immersive ? 'hideBar' : ''">
      <span v-if="channelInfo.timeLeft">剩余时长：{{ parseTime(timeLeft) }}</span>
      <NetworkDetector />
      <Row class="fullscreen">
        <Tooltip
          :arrow="false"
          placement="bottomLeft"
          overlayClassName="viewConfigContainer"
          :overlay-inner-style="{ backgroundColor: 'rgba(245, 247, 250, 0.9)' }"
        >
          <Col class="viewConfigHot">
            <Icon :type="channelInfo.mode === 'grid' ? 'icon-gallery_20' : 'icon-speaker_top_20'" />
            视图
          </Col>
          <template #title>
            <Row class="viewConfig">
              <RadioGroup v-model:value="channelInfo.mode">
                <Radio value="standard">
                  <Icon type="icon-speaker_top_20" />
                  标准
                </Radio>
                <Radio value="grid">
                  <Icon type="icon-gallery_20" />
                  宫格
                </Radio>
              </RadioGroup>
              <Divider />
              <Col @click="onFullScreen">
                {{ fullscreen ? '退出全屏' : '全屏' }}
              </Col>
            </Row>
          </template>
        </Tooltip>
        <Divider type="vertical" v-if="fullscreen"/>
        <Icon class="viewConfigHot" v-if="fullscreen" @click="onFullScreen" type="icon-XDS_Minimize" />
      </Row>
    </Row>
    <Space v-if="channelInfo.mode === 'standard'" :class="['smallVideoItems', channelInfo.mode]">
      <SmallView v-for="user in channelInfo.allUsers" :key="user.userId" :user="user" :track="channelInfo.getTrack(user)" />
    </Space>
    <Row v-else :class="['smallVideoItems', channelInfo.mode]">
      <SmallView
        v-for="user in channelInfo.allUsers"
        :span="spanNum"
        :style="{ height: gridHeight + 'px' }"
        :key="user.userId"
        :user="user"
        :track="channelInfo.getTrack(user)"
      />
    </Row>
    <MainView v-if="channelInfo.mode === 'standard'" />
    <ToolBar @leave="clearRoom" />
  </Row>
</template>

<style lang="less" scoped>
@import url('./index.module.less');
</style>
