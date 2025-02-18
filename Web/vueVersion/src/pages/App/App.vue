<template>
  <div class="wrap">
    <template v-if="supported">
      <InChannel v-if="globalFlag.joined" />
      <Welcome v-else />
    </template>
  </div>
</template>

<script setup lang="ts">
import { onMounted, onUnmounted, ref } from "vue";
import InChannel from "../InChannel/InChannel.vue";
import Welcome from "~/pages/Welcome/index.vue";
import { useChannelInfo, useGlobalFlag } from "~/store";
import { message, Modal } from 'ant-design-vue';
import DingRTC from "dingrtc";
import VConsole from "vconsole";
import { useDevice } from "~/hooks/device";
import 'ant-design-vue/dist/reset.css';
import { defaultWhiteboardId } from "~/constants/index";
import { useWhiteboardHooks } from "~/hooks/channel";


const globalFlag = useGlobalFlag();
const { updateDeviceList } = useDevice();
const channelInfo = useChannelInfo();
const supported = ref(false);
const { openWhiteboard } = useWhiteboardHooks();


// 生命周期钩子
onMounted(() => {
  if (!globalFlag.hideLog && globalFlag.isMobile) {
    new VConsole();
  }

  const support = DingRTC.checkSystemRequirements();
  supported.value = support;
  if (!supported) {
    message.error("当前浏览器不支持通话");
  }

  channelInfo.whiteboardManager.on('whiteboard-start', (id) => {
    if (id === defaultWhiteboardId) {
      openWhiteboard();
    }
  })

  DingRTC.on("camera-changed", (info) => {
    updateDeviceList("camera", info);
  });

  DingRTC.on("microphone-changed", (info) => {
    updateDeviceList("microphone", info);
  });

  DingRTC.on("playback-device-changed", (info) => {
    updateDeviceList("playback-device", info);
  });

  DingRTC.on("autoplay-failed", (track) => {
    Modal.centerConfirm({
      content: "由于浏览器自动播放限制，点击确定后才会开始播放音频",
      onOk() {
        // @ts-ignore
        track.play();
      },
    });
  });
});

onUnmounted(() => {
  DingRTC.removeAllListeners();
});
</script>

<style lang="less" scoped>
@import url("./index.module.less");
</style>