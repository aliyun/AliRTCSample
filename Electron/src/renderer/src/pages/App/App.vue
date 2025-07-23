<template>
  <div class="wrap">
    <InChannel v-if="globalFlag.joined" />
    <Welcome v-else />
  </div>
</template>

<script setup lang="ts">
import { onMounted, onUnmounted } from "vue";
import InChannel from "../InChannel/InChannel.vue";
import Welcome from "~/pages/Welcome/index.vue";
import { useGlobalFlag } from "~/store";
import DingRTC from "dingrtc-electron-sdk";
import { useDevice } from "~/hooks/device";
import 'ant-design-vue/dist/reset.css';


const globalFlag = useGlobalFlag();
const { updateDeviceList } = useDevice();


// 生命周期钩子
onMounted(() => {

  // const supported = DingRTC.checkSystemRequirements();
  // if (!supported) {
  //   message.error("当前浏览器不支持通话");
  // }

  DingRTC.on("camera-changed", (info) => {
    updateDeviceList("camera", info);
  });

  DingRTC.on("microphone-changed", (info) => {
    updateDeviceList("microphone", info);
  });

  DingRTC.on("playback-device-changed", (info) => {
    updateDeviceList("playback-device", info);
  });

});

onUnmounted(() => {
  DingRTC.removeAllListeners();
});
</script>

<style lang="less" scoped>
@import url("./index.module.less");
</style>