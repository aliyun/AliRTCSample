<template>
  <div class="wrap">
    <Title :level="5" class="demoTitle">DingRTC Demo</Title>
    <Button 
      :disabled="testing" 
      class="testNetwork" 
      @click="onTestNetwork"
    >
      {{ testing ? `请等待${ticktok}s` : '测试网络' }}
    </Button>
    <div class="main">
      <Preview />
      <Join />
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from "vue";
import { Typography, Button } from 'ant-design-vue';
import Preview from "./components/Preview.vue";
import Join from "./components/Join.vue";
import { startTestNetworkQuality } from "~/utils/networkQuality";

const { Title } = Typography;

// 状态管理
const testing = ref(false);
const ticktok = ref(15);

// 方法
const onTestNetwork = async () => {
  if (!testing.value) {
    testing.value = true;
    ticktok.value = 15;

    const intervalId = setInterval(() => {
      ticktok.value = Math.max(0, ticktok.value - 1);
    }, 1000);

    await startTestNetworkQuality();

    testing.value = false;
    clearInterval(intervalId);
  }
};
</script>

<style lang="less" scoped>
@import url('./index.module.less');
</style>