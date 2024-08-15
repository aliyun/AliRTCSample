<script setup lang="ts">
import GapLine from '@src/components/GapLine.vue';

const props = defineProps<{ title: string }>();
</script>
<template>
  <div class="tabItemWrapper">
    <div class="title">{{ props.title }}</div>
    <GapLine />
    <slot name="content"></slot>
  </div>
</template>
<style lang="less" scoped>
.tabItemWrapper {
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  .title {
    color: #171a1d;
    font-size: 18px;
    font-weight: 600;
    line-height: 25px;
  }
  .contentWrapper {
    flex: 1;
  }
}
</style>
