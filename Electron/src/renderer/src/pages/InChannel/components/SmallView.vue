<template>
  <Col
    ref="containerRef"
    :style="style"
    :id="'uid_' + user.userId"
    :class="[
      'smallVideoItem',
      {
        avatar: !videoIsPlay,
        camera: videoIsPlay,
        speaking: channelInfo.speakers.includes(user.userId),
      },
    ]"
    :span="span"
    @dblclick="viewBigger"
  >
    <span class="userId"> uid:{{ user.userId }} </span>
    <component
      :is="ScoreMap(channelInfo.remoteUserNetworkQualitys[user.userId] || 0)?.icon"
      v-if="!isLocal && track"
    />
    <Avatar size="large">
      {{ user.userName }}
    </Avatar>
    <div class="smallViewStatus">
      <Icon v-if="channelInfo.trackStatsMap.get(props.user.userId)?.screen" style="color: limegreen" type="icon-XDS_share_screen1" />
      <Icon v-else-if="!micIconEnable" type="icon-XDS_UnMute2Fill" />
      <Icon v-else type="icon-XDS_Mute2" />
      <Tooltip :title="user.userName">
        <span>{{ user.userName }}</span>
      </Tooltip>
    </div>
    <Tooltip :key="actionsKey" :arrow="false" :overlay-inner-style="{ backgroundColor: 'rgba(245, 247, 250, 0.9)' }">
      <template #title>
        <List>
          <List.Item
            v-for="item in actions"
            :key="item.text"
            class="actionsItem"
            @click="item.onClick"
          >
            {{ item.text }}
          </List.Item>
        </List>
      </template>
      <div class="smallViewActions">
        <Icon type="icon-XDS_List" />
      </div>
    </Tooltip>
  </Col>
</template>
<script lang="ts" setup>
import { Avatar, Col, Tooltip, List } from 'ant-design-vue';
import Icon from '~/components/Icon';
import { useClient, useChannelInfo, useCurrentUserInfo, useDeviceInfo } from '~/store';
import { ScoreMap } from './NetworkBar';
import { computed, ref, toRaw, watch } from 'vue';
import { useChannel } from '~/hooks/channel';
const os = require('os');
const path = require('path');

const downloadPath = path.join(os.homedir(), 'Downloads')

const props = defineProps(['user', 'span', 'style', 'track']);
const { subscribe, unsubscribe } = useChannel();
const user = ref(props.user);
const channelInfo = useChannelInfo();
const deviceInfo = useDeviceInfo();
const currentUserInfo = useCurrentUserInfo();
const client = useClient();
const streamType = ref(channelInfo.defaultRemoteStreamType);
const containerRef = ref(null);
const actionsKey = ref(1);

const isLocal = computed(() => props.user.userId === currentUserInfo.userId);

const micIconEnable = computed(() => {
  if (isLocal.value) return deviceInfo.micEnable;
  return channelInfo.trackStatsMap.get(props.user.userId)?.mic;
})

const videoIsPlay = computed(() => {
  let track = props.track;
  const trackStats = channelInfo.trackStatsMap.get(props.user.userId);
  if (!track) return false;
  if (track === props.user.videoTrack) return trackStats?.camera;
  if (track === props.user.auxiliaryTrack) return trackStats?.screen;
  return false;
});

watch(() => [containerRef.value?.$el, props.track, videoIsPlay], (newValue) => {
  if (newValue[0] && newValue[1]) {
    toRaw(newValue[1]).play(newValue[0], {
      fit: 'contain',
    });
  }
})

const viewBigger = () => {
  if (channelInfo.mode === 'grid') {
    channelInfo.mode = 'standard';
  }
  channelInfo.$patch((state) => {
    if (state.mainViewUserId === props.user.userId) {
      state.mainViewPreferType = state.mainViewPreferType === 'camera' ? 'auxiliary' : 'camera';
    } else {
      state.mainViewUserId = props.user.userId;
      state.mainViewPreferType = 'camera';
    }
  });
};

const actions = computed(() => {
  const uid = props.user.userId;
  const buttons = [
    {
      text: '截图',
      show: props.track,
      onClick: () => {
        toRaw(props.track).getCurrentFrameData(`${downloadPath}/${uid}_${Date.now()}.jpg`);
      },
    },
    {
      text: '订阅摄像头',
      show: !isLocal.value && channelInfo.trackStatsMap.get(uid).hasCamera && !channelInfo.trackStatsMap.get(uid).subscribedCamera,
      onClick: () => {
        subscribe(props.user, 'video')
      },
    },
    {
      text: '取消订阅摄像头',
      show: !isLocal.value && channelInfo.trackStatsMap.get(uid).subscribedCamera,
      onClick: () => {
        unsubscribe(props.user, 'video')
      },
    },
    {
      text: '订阅共享',
      show: !isLocal.value && channelInfo.trackStatsMap.get(uid).hasScreen && !channelInfo.trackStatsMap.get(uid).subscribedScreen,
      onClick: () => {
        subscribe(props.user, 'video', true);
      },
    },
    {
      text: '取消订阅共享',
      show: !isLocal.value && channelInfo.trackStatsMap.get(uid).subscribedScreen,
      onClick: () => {
        unsubscribe(props.user, 'video', true);
      },
    },
    {
      text: '切FHD',
      show: !isLocal.value && props.track && streamType.value !== 'FHD'  && props.track === props.user.videoTrack,
      onClick: () => {
        toRaw(client).setRemoteVideoStreamType(props.user.userId, 'FHD').then(() => {
          streamType.value = 'FHD';
        });
      },
    },
    {
      text: '切HD',
      show: !isLocal.value && props.track && streamType.value !== 'HD' && props.track === props.user.videoTrack,
      onClick: () => {
        toRaw(client).setRemoteVideoStreamType(props.user.userId, 'HD').then(() => {
          streamType.value = 'HD';
        });
      },
    },
    {
      text: '切SD',
      show: !isLocal.value && props.track && streamType.value !== 'SD' && props.track === props.user.videoTrack,
      onClick: () => {
        toRaw(client).setRemoteVideoStreamType(props.user.userId, 'SD').then(() => {
          streamType.value = 'SD';
        });
      },
    },
    {
      text: '切LD',
      show: !isLocal.value && props.track && streamType.value !== 'LD' && props.track === props.user.videoTrack,
      onClick: () => {
        toRaw(client).setRemoteVideoStreamType(props.user.userId, 'LD').then(() => {
          streamType.value = 'LD';
        });
      },
    },
  ].filter((item) => item.show)
  return buttons;
});
</script>
<style lang="less" scoped>
@import url('../index.module.less');
</style>
