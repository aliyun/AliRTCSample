<script lang="ts" setup></script>
<template>
  <div class="background">
    <div class="logo">DingRTC Demo</div>
    <slot></slot>
  </div>
</template>
<style scoped>
.background {
  width: 100vw;
  height: 100vh;
  background: url('https://img.alicdn.com/imgextra/i2/O1CN01rLjlAZ1hOnKqJqU0I_!!6000000004268-0-tps-3840-2160.jpg')
    center center / cover;
}
.logo {
  display: flex;
  align-items: center;
  left: 40px;
  position: absolute;
  top: 40px;
  font-size: 20px;
}
</style>
