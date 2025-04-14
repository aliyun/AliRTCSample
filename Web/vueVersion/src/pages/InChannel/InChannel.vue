<script lang="ts" setup>
import { ref, onMounted, onUnmounted, computed, watch, watchEffect } from 'vue';
import SmallView from './components/SmallView.vue';
import MainView from './components/MainView.vue';
import Whiteboard from './components/Whiteboard.vue';
import ToolBar from './components/ToolBar.vue';
import { NetworkDetector } from './components/NetworkBar';
import Icon from '~/components/Icon';
import { useChannel, useNetworkStats } from '~/hooks/channel';
import { parseTime, print } from '~/utils/tools';
import { Divider, Col, message, Radio, RadioGroup, Row, Space, Tooltip } from 'ant-design-vue';
import {
  Group,
  GroupPropertyUpdateTypes,
  GroupUser,
  LocalVideoTrack,
  NetworkQuality,
} from 'dingrtc';
import { useClient, useGlobalFlag, useChannelInfo, useCurrentUserInfo, useDeviceInfo, useRTMInfo } from '~/store';
import ChatRoom from './components/ChatRoom/index.vue';

const rtcStatsTimer = ref(0);
const wrapRef = ref(null);
const timeLeftTimer = ref(0);
const fullscreen = ref(false);
const channelInfo = useChannelInfo();
const rtmInfo = useRTMInfo();
const client = useClient();
const globalFlag = useGlobalFlag();
const deviceInfo = useDeviceInfo();
const currentUserInfo = useCurrentUserInfo();
const { subscribe, checkMainview } = useChannel();
const { getRtcStats, getRemoteUserNetworkStats } = useNetworkStats();
const timeLeft = ref(channelInfo.timeLeft - 1);
const gridHeight = ref(0);

const immersiveTimer = ref(0);

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

const clearRoom = () => {
  if (document.fullscreenElement) {
    document.exitFullscreen();
  }
  if (channelInfo.cameraTrack) {
    channelInfo.cameraTrack.close();
  }
  if (channelInfo.micTrack) {
    channelInfo.micTrack.close();
  }
  if (channelInfo.screenTrack) {
    channelInfo.screenTrack.close();
  }
  if (channelInfo.mcuAudioTrack) {
    channelInfo.mcuAudioTrack.stop();
  }

  channelInfo.remoteUsers.forEach((user) => {
    if (user.auxiliaryTrack) {
      user.auxiliaryTrack.stop();
    }
    if (user.videoTrack) {
      user.videoTrack.stop();
    }
  });
  rtmInfo.rtm.detach();
  deviceInfo.$reset();
  channelInfo.whiteboardManager?.clear();
  channelInfo.$reset();
  globalFlag.$reset();
  rtmInfo.$patch({
    enabled: false,
    sessions: [],
    activeSessionId: '',
  })
};

onMounted(() => {
  getRtcStats();
  getRemoteUserNetworkStats();
  document.addEventListener('fullscreenchange', onExitFullScreen);
  rtcStatsTimer.value = window.setInterval(() => {
    getRtcStats();
    getRemoteUserNetworkStats();
  }, 2000);
  timeLeftTimer.value = window.setInterval(() => {
    timeLeft.value = Math.max(timeLeft.value - 1, 0);
  }, 1000);
  client.on('user-unpublished', (user, mediaType, auxiliary) => {
    print(
      `user ${user.userId} unpublished ${mediaType === 'audio' ? 'audio' : auxiliary ? 'screenShare' : mediaType}}`,
    );
    channelInfo.updateTrackStats(user.userId);
    channelInfo.$patch({ remoteUsers: client.remoteUsers });
  });
  client.on('user-published', (user, mediaType, auxiliary) => {
    print(
      `user ${user.userId} ${user.id} published ${mediaType === 'audio' ? 'audio' : auxiliary ? 'screenShare' : mediaType}}`,
    );
    channelInfo.updateTrackStats(user.userId);
    channelInfo.$patch({ remoteUsers: client.remoteUsers });
    if (mediaType !== 'video') {
      return;
    }
    if (channelInfo.subscribeAllVideo) {
      subscribe(user, 'video', auxiliary);
    }
  });
  client.on('user-joined', (user) => {
    print(`user ${user.userId} joined`, user);
    channelInfo.$patch({ remoteUsers: client.remoteUsers });
    channelInfo.updateTrackStats(user.userId);
  });
  client.on('stream-type-changed', (uid, streamType) => {
    print(`user ${uid} streamType changeTo ${streamType}`);
  });

  client.on('connection-state-change', (current, _, reason) => {
    print(`connection-state-change ${current} ${reason || ''}`);
    if (current === 'disconnected') {
      if (reason !== 'leave') {
        message.info(reason);
      }
      clearRoom();
    }
  });
  client.on('network-quality', (uplink, downlink) => {
    channelInfo.$patch({ networkQuality: Math.max(uplink, downlink) as NetworkQuality });
  });
  client.on('volume-indicator', (uids: string[]) => {
    if (uids.length) {
      print(`${uids.join()} is speaking`);
    }
    channelInfo.$patch({ speakers: uids });
  });
  client.on('user-info-updated', (uid, msg) => {
    print(`user ${uid}: ${msg}`);
    channelInfo.$patch({ remoteUsers: client.remoteUsers });
    channelInfo.updateTrackStats(uid);
  });
  client.on('user-left', (user) => {
    print(`user ${user.userId} left`);
    checkMainview(user);
    channelInfo.updateTrackStats(user.userId);
    channelInfo.$patch({ remoteUsers: client.remoteUsers });
  });
  client.on('group-add', (group: Group) => {
    print(`group add`, group);
    channelInfo.groups = client.groups;
  });
  client.on('group-remove', (group: Group) => {
    print(`group remove`, group);
    if (channelInfo.subscribeAudio === group.id) {
      channelInfo.subscribeAudio = '';
    }
    channelInfo.$patch({ groups: client.groups });
  });
  client.on('group-user-join', (groupId: string, user: GroupUser) => {
    print(`group-user-join`, groupId, user);
    channelInfo.$patch({ groups: client.groups });
  });
  client.on('group-user-leave', (groupId: string, user: GroupUser) => {
    print(`group-user-leave`, groupId, user);
    channelInfo.$patch({ groups: client.groups });
  });
  client.on(
    'group-info-updated',
    (groupId: string, event: GroupPropertyUpdateTypes, value?: string) => {
      print(`group-info-updated`, groupId, event, value);
      channelInfo.$patch({ groups: client.groups });
    },
  );
  document.addEventListener('mousemove', hideAfterStill);
});

watch(
  () => channelInfo.screenTrack,
  () => {
    channelInfo.updateTrackStats(currentUserInfo.userId);
  },
  {
    immediate: true,
  },
);

watchEffect((cleanUp) => {
  if (!channelInfo.screenTrack) return;
  channelInfo.screenTrack?.on('track-ended', clearTrack);
  cleanUp(() => {
    channelInfo.screenTrack?.off('track-ended', clearTrack);
  });
});

onUnmounted(() => {
  clearInterval(rtcStatsTimer.value);
  clearInterval(timeLeftTimer.value);
  clearTimeout(immersiveTimer.value);
  document.removeEventListener('mousemove', hideAfterStill);
  client.removeAllListeners();
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
              <RadioGroup
                v-model:value="channelInfo.mode"
                :disabled="channelInfo.isWhiteboardOpen"
              >
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
        <Divider type="vertical" v-if="fullscreen" />
        <Icon
          class="viewConfigHot"
          v-if="fullscreen"
          @click="onFullScreen"
          type="icon-XDS_Minimize"
        />
      </Row>
    </Row>
    <Space v-if="channelInfo.mode === 'standard'" :class="['smallVideoItems', channelInfo.mode]">
      <SmallView
        v-for="user in channelInfo.allUsers"
        :key="user.userId"
        :user="user"
        :track="channelInfo.getTrack(user)"
      />
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
    <MainView v-if="channelInfo.mode === 'standard' && !channelInfo.isWhiteboardOpen" />
    <Whiteboard v-if="channelInfo.mode === 'standard' && channelInfo.isWhiteboardOpen" />
    <ToolBar @leave="clearRoom" />
    <ChatRoom />
  </Row>
</template>

<style lang="less" scoped>
@import url('./index.module.less');
</style>
