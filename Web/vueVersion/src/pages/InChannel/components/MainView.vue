<script setup lang="ts">
import { computed, onBeforeUnmount, ref, watch } from 'vue';
import { Tooltip, Row, Avatar } from 'ant-design-vue';
import { useGlobalFlag, useChannelInfo, useDeviceInfo, useCurrentUserInfo } from '~/store';
import Icon from '~/components/Icon';
import Annotation from './Annotation.vue';
import { logger } from '~/utils/tools';

const viewRef = ref(null);

// 状态管理
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

watch(() => [viewRef.value, channelInfo.mainViewTrack], (newValue, oldValue) => {
  logger.info('newValue, oldValue=====', newValue, oldValue);
  if (newValue[1]) {
    newValue[1].play(viewRef.value.$el, {
      fit: 'contain',
    });
  }
  if (oldValue?.[0]?.$el && oldValue?.[1] && oldValue?.[0]?.$el !== oldValue?.[1]?.$el) {
    oldValue[1].stop(oldValue?.[0]?.$el);
  }
})


onBeforeUnmount(() => {
  if (channelInfo.mainViewTrack && viewRef.value?.$el) {
    logger.info('Stopping track play for mainview');
    channelInfo.mainViewTrack.stop(viewRef.value.$el);
  }
});


</script>
<template>
  <Row ref="viewRef" :class="['mainView', { avatar: !channelInfo.mainViewTrack }]">
    <Annotation v-if="channelInfo.annotation" />
    <div v-if="channelInfo.mainViewTrack" :class="['smallViewStatus', { higherMainUserInfo: !globalFlag.immersive }]">
      <Icon v-if="channelInfo.trackStatsMap.get(channelInfo.mainViewUserId)?.screen" style="color: limegreen;"
        type="icon-XDS_share_screen1" />
      <Icon v-if="!micIconEnable" type="icon-XDS_UnMute2Fill" />
      <Tooltip :title="showName">
        <span>{{ showName }}</span>
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
