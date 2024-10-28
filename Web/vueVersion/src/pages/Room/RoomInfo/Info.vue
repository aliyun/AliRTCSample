<script setup lang="ts">
import { useCurrentUserInfo } from '@src/store/currentUserInfo';

const userInfo = useCurrentUserInfo();
</script>
<template>
  <div :style="{ position: 'absolute', zIndex: 21, left: '5px', bottom: '1px', fontSize: '14px' }">
    <div>应用: {{ userInfo.appId }}</div>
    <div>环境: {{ 'pre' }}</div>
    <div>频道: {{ userInfo.channel }}</div>
  </div>
</template>
