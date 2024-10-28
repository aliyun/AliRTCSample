<script setup lang="ts">
import Background from '@src/components/Background.vue';

import JoinForm from './JoinForm/index.vue';
import Preview from './Preview/index.vue';
</script>

<template>
  <Background>
    <div class="main-container">
      <Preview />
      <JoinForm />
    </div>
  </Background>
</template>

<style scoped>
.main-container {
  width: 100%;
  max-height: 800px;
  height: 60vh;
  opacity: 1;
  /* 自动布局 */
  display: flex;
  justify-content: center;
  align-items: center;
  padding: 0px;
  gap: 20px;
  width: 90vw;
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  margin: auto;
}
@media (max-width: 768px) {
  /* 当屏幕宽度小于或等于 768px 时，改为上下布局 */
  .main-container {
    flex-direction: column; /* 切换 flex 方向为垂直堆叠 */
    height: 85vh;
  }
}
</style>
