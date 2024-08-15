<script setup lang="ts">
import { onMounted, ref } from 'vue';

import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { parseTime } from '@src/utils/tools';

const localChannelInfo = useLocalChannelInfoStore();
const timeLeft = ref(localChannelInfo.timeLeft || 0);
onMounted(() => {
  const timer = setInterval(() => {
    timeLeft.value = Math.max(timeLeft.value - 1, 0);
  }, 1000);
  return () => {
    clearInterval(timer);
  };
});
</script>
<template>
  <span
    :style="{
      position: 'absolute',
      left: '5px',
      top: '5px',
      zIndex: 1,
      fontSize: '14px',
    }"
    >剩余时长：{{ parseTime(timeLeft * 1000) }}</span
  >
</template>
