<template>
  <div style="width: 100%">
    <Progress size="small" :percent="level || width" :steps="95" :show-info="false" />
  </div>
</template>
<script setup lang="ts">
import { Progress } from 'ant-design-vue';
import { onUnmounted, ref, toRaw, watch } from 'vue';

const props = defineProps(['track', 'level']);

const interval = ref(0);
const width = ref(0);
watch(() => props.track, (newValue) => {
  if (!newValue) return;
  interval.value = window.setInterval(() => {
    width.value = toRaw(newValue)?.getVolumeLevel() * 100;
  }, 400);
}, { immediate: true })

onUnmounted(() => {
  clearInterval(interval.value);
})

</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
