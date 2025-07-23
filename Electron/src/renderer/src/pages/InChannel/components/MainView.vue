<script setup lang="ts">
import { computed, ref, toRaw, watch } from 'vue';
import { Tooltip, Row, Avatar } from 'ant-design-vue';
import { useGlobalFlag, useChannelInfo, useDeviceInfo, useCurrentUserInfo } from '~/store';
import Icon from '~/components/Icon';

const viewRef = ref(null);

const globalFlag = useGlobalFlag();
const channelInfo = useChannelInfo();
const deviceInfo = useDeviceInfo();
const currentUserInfo = useCurrentUserInfo();

const micIconEnable = computed(() => {
  const isLocal = channelInfo.mainViewUserId === currentUserInfo.userId;
  if (isLocal) return deviceInfo.micEnable;
  return channelInfo.trackStatsMap.get(channelInfo.mainViewUserId)?.mic;
})

const showName = computed(() => channelInfo.mainViewUserInfo?.userName || currentUserInfo.userName)

watch(() => [viewRef.value, channelInfo.mainViewTrack], (newValue) => {
  if (newValue[1]) {
    toRaw(newValue[1]).play(viewRef.value.$el, {
      fit: 'contain',
    });
  }
})
</script>
<template>
  <Row ref="viewRef" :class="['mainView', { avatar: !channelInfo.mainViewTrack }]">
    <div v-if="channelInfo.mainViewTrack" :class="['smallViewStatus', { higherMainUserInfo: !globalFlag.immersive }]">
      <Icon v-if="channelInfo.trackStatsMap.get(channelInfo.mainViewUserId)?.screen" style="color: limegreen;" type="icon-XDS_share_screen1" />
      <Icon v-if="!micIconEnable" type="icon-XDS_UnMute2Fill" />
      <Tooltip :title="channelInfo.mainViewUserInfo.userName">
        <span>{{ channelInfo.mainViewUserInfo.userName }}</span>
      </Tooltip>
    </div>
    <Avatar size="large">
      {{ showName }}
    </Avatar>
  </Row>
</template>
<style lang="less" scoped>
@import url('../index.module.less');
</style>