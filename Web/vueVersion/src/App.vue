<script setup lang="ts">
import DingRTC from 'dingrtc';
import { onMounted } from 'vue';

import { isInRoom } from '@src/Router';

import Room from './pages/Room/index.vue';
import Welcome from './pages/index.vue';
import { useConstanceConfig } from './store/constantConfig';

const { hideLog, isMobile } = useConstanceConfig();
onMounted(() => {
  DingRTC.setLogLevel('debug');
});
</script>

<template>
  <Welcome v-if="!isInRoom" />
  <Room v-else />
  <div v-if="!hideLog && !isMobile"><OperationLog /></div>
</template>

<style scoped>
.logo {
  height: 6em;
  padding: 1.5em;
  will-change: filter;
  transition: filter 300ms;
}
.logo:hover {
  filter: drop-shadow(0 0 2em #646cffaa);
}
.logo.vue:hover {
  filter: drop-shadow(0 0 2em #42b883aa);
}
</style>
