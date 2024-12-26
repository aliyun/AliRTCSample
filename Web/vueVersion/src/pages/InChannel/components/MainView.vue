<script setup lang="ts">
import { computed, ref, watch } from 'vue';
import { Tooltip, Row, Avatar } from 'ant-design-vue';
import { useGlobalFlag, useChannelInfo, useDeviceInfo, useCurrentUserInfo } from '~/store';
import Icon from '~/components/Icon';

const viewRef = ref(null);

// 状态管理
const globalFlag = useGlobalFlag();
const channelInfo = useChannelInfo();
const deviceInfo = useDeviceInfo();
const currentUserInfo = useCurrentUserInfo();

const micIconEnable = computed(() => {
  const isLocal = channelInfo.mainViewUserId === currentUserInfo.userId;
  if (isLocal) return deviceInfo.micEnable;
  return channelInfo.trackStatsMap.get(channelInfo.mainViewUserId).mic;
})


const videoIsPlay = computed(() => {
  const trackStats = channelInfo.trackStatsMap.get(channelInfo.mainViewUserId);
  console.log(trackStats, channelInfo.mainViewPreferType)
  return channelInfo.mainViewPreferType === 'auxiliary' ? trackStats.screen : trackStats.camera;
});

watch(() => [viewRef.value, channelInfo.mainViewTrack], (newValue) => {
  if (newValue[1]) {
    newValue[1].play(viewRef.value.$el, {
      fit: 'contain',
    });
  }
})
</script>
<template>
  <Row ref="viewRef" :class="['mainView', { avatar: !videoIsPlay }]">
    <div v-if="channelInfo.mainViewTrack" :class="['smallViewStatus', { higherMainUserInfo: !globalFlag.immersive }]">
      <Icon v-if="channelInfo.trackStatsMap.get(channelInfo.mainViewUserId)?.screen" style="color: limegreen;" type="icon-XDS_share_screen1" />
      <Icon v-if="!micIconEnable" type="icon-XDS_UnMute2Fill" />
      <Tooltip :title="channelInfo.mainViewUserInfo.userName">
        <span>{{ channelInfo.mainViewUserInfo.userName }}</span>
      </Tooltip>
    </div>
    <Avatar size="large">
      {{ channelInfo.mainViewUserInfo.userName }}
    </Avatar>
  </Row>
</template>
<style lang="less" scoped>
@import url('../index.module.less');
</style>