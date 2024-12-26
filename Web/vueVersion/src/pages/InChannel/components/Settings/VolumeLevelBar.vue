<template>
  <div style="width: 100%">
    <Progress size="small" :percent="width" :steps="80" :show-info="false" />
  </div>
</template>
<script setup lang="ts">
import { Progress } from 'ant-design-vue';
import { ref, watchEffect } from 'vue';

const props = defineProps(['track']);

const interval = ref(0);
const width = ref(0);
watchEffect((cleanUp) => {
  if (!props.track) return;
  interval.value = window.setInterval(() => {
    width.value = props.track.getVolumeLevel() * 100;
  }, 400);
  cleanUp(() => {
    clearInterval(interval.value);
  })
});
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
