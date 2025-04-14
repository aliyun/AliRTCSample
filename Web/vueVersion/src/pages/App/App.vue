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
import { useChannelInfo, useGlobalFlag, useRTMInfo } from "~/store";
import { message, Modal } from 'ant-design-vue';
import DingRTC from "dingrtc";
import VConsole from "vconsole";
import { useDevice } from "~/hooks/device";
import 'ant-design-vue/dist/reset.css';
import { useWhiteboardHooks } from "~/hooks/channel";
import { print } from "~/utils/tools";

const globalFlag = useGlobalFlag();
const { updateDeviceList, getDeviceList } = useDevice();
const channelInfo = useChannelInfo();
const supported = ref(false);
const { openWhiteboard, openAnnotation, closeAnnotation, closeWhiteboard } = useWhiteboardHooks();
const rtmInfo = useRTMInfo();

// 生命周期钩子
onMounted(() => {
  if (!globalFlag.hideLog && globalFlag.isMobile) {
    new VConsole();
  }
  DingRTC.setLogLevel('debug');
  const support = DingRTC.checkSystemRequirements();
  supported.value = support;
  if (!supported) {
    message.error("当前浏览器不支持通话");
  }
  if (globalFlag.isMobile) {
    getDeviceList(true, true);
  }
  channelInfo.whiteboardManager.on('whiteboard-start', (id) => {
    openWhiteboard(id);
  })

  channelInfo.whiteboardManager.on('annotation-start', (id, sourceType) => {
    openAnnotation(id, sourceType);
  })

  channelInfo.whiteboardManager.on('annotation-stop', (id, sourceType) => {
    closeAnnotation(id, sourceType);
  })

  channelInfo.whiteboardManager.on('whiteboard-stop', (id) => {
    closeWhiteboard(id);
  })

  rtmInfo.rtm.on('message', (data) => {
    const session = rtmInfo.sessions.find(item => item.sessionId === data.sessionId)
    session.messages.push({
      ...data,
      timestamp: Date.now(),
    });
  })
  rtmInfo.rtm.on('session-add', (session) => {
    print(`rtm session add`, session);
    const idx = rtmInfo.sessions.findIndex(item => item.sessionId === session.id)
    if (idx === -1) {
      rtmInfo.sessions.push({
        sessionId: session.id,
        members: [...session.users],
        messages: [],
      });
    }
  })
  rtmInfo.rtm.on('session-remove', (session) => {
    print(`rtm session remove`, session);
    const idx = rtmInfo.sessions.findIndex(item => item.sessionId === session.id)
    rtmInfo.sessions.splice(idx, 1);
  })
  rtmInfo.rtm.on('session-user-join', (sessionId, uid) => {
    print(`rtm session user join`, sessionId, uid);
    const session = rtmInfo.sessions.find(item => item.sessionId === sessionId)
    const idx = session.members.findIndex(item => uid === item.userId)
    if (idx === -1) {
      // @ts-ignore
      session.members.push({ userId: uid })
    }
  })
  rtmInfo.rtm.on('session-user-left', (sessionId, uid) => {
    print(`rtm session user left`, sessionId, uid);
    const session = rtmInfo.sessions.find(item => item.sessionId === sessionId)
    const idx = session.members.findIndex(item => uid === item.userId)
    session.members.splice(idx, 1);
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