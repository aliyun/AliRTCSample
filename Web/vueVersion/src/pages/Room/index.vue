<script setup lang="ts">
import DingRTC from 'dingrtc';
import { onMounted, ref } from 'vue';

import { useClient } from '@src/store/client';

import BottomBar from './BottomBar/index.vue';
import MainView from './MainView/index.vue';
import Info from './RoomInfo/Info.vue';
import Timer from './RoomInfo/Timer.vue';
import SmallView from './SmallViews/index.vue';
import { useRoomListener } from './useRoomListener';
import { useScreenTrackListener } from './useSreenTrackListener';

const client = useClient();

const customVideo = ref<HTMLVideoElement>();

onMounted(() => {
  if (!customVideo.value) return;
  customVideo.value.onplay = async () => {
    // 这个问题的原因是，尽管 captureStream 方法在现代浏览器中是受支持的，但它在 TypeScript 的标准 DOM 类型定义中可能还没有被包括。
    const videoTrack = (customVideo.value as HTMLVideoElement)?.captureStream().getVideoTracks()[0];
    const customTrack = await DingRTC.createCustomVideoTrack({
      mediaStreamTrack: videoTrack!,
      frameRate: 45,
      dimension: 'VD_1920x1080',
    });
    client.publish(customTrack);
  };
});

useRoomListener();
useScreenTrackListener();
</script>

<template>
  <div class="roomViewWrapper">
    <Timer />
    <SmallView />
    <div class="mainView"><MainView /></div>
    <Info />
    <BottomBar />
  </div>
</template>

<style scoped>
.roomViewWrapper {
  width: 100vw;
  height: 100vh;
  display: flex;
  flex-direction: column;
  background-color: #dce1e6;
}
.mainView {
  flex: 1;
  position: relative;
  display: flex;
  justify-content: center;
  align-items: center;
}
</style>
