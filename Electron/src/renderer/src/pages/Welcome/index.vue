<template>
  <div class="wrap">
    <Title :level="5" class="demoTitle">DingRTC Demo</Title>
    <div class="main">
      <Preview />
      <Join />
    </div>
  </div>
</template>

<script setup lang="ts">
import { Typography } from 'ant-design-vue';
import Preview from "./components/Preview.vue";
import Join from "./components/Join.vue";

const { Title } = Typography;


</script>

<style lang="less" scoped>
@import url('./index.module.less');
</style>