<script setup lang="ts">
import 'ant-design-vue/dist/reset.css';
import DingRTC from 'dingrtc';
import { computed } from 'vue';

import Welcome from '@src/pages/Welcome/index.vue';

import { useInitOneRTC } from './hooks';

const support = computed(() => DingRTC.checkSystemRequirements());
useInitOneRTC();
</script>

<template>
  <div v-if="support">
    <Welcome />
  </div>
  <div v-else>暂不支持</div>
</template>

<style scoped>
.read-the-docs {
  color: #888;
}
</style>
