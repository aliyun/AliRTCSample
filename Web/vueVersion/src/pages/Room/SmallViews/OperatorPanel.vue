<script setup lang="ts">
import { Popover, Button } from 'ant-design-vue';
import { RemoteUser } from 'dingrtc';

import { EllipsisOutlined } from '@ant-design/icons-vue';
import { useRemoteChannel } from '@src/hooks/useRemoteChannel';

const props = defineProps<{ isLocal: boolean; user: RemoteUser }>();
const { subscribe, unSubscribe } = useRemoteChannel();
const { isLocal, user } = props;
const btns = [
  {
    text: '订阅摄像头',
    show: !isLocal && user.hasVideo && !user.videoTrack,
    onClick: () => {
      subscribe(user, 'video');
    },
  },
  {
    text: '订阅共享',
    show: !isLocal && user.hasAuxiliary && !user.auxiliaryTrack,
    onClick: () => {
      subscribe(user, 'video', true);
    },
  },
  {
    text: '取消订阅共享',
    show: !isLocal && user.auxiliaryTrack,
    onClick: () => {
      unSubscribe(user, 'video', true);
    },
  },
  {
    text: '取消订阅摄像头',
    show: !isLocal && user.videoTrack,
    onClick: () => {
      unSubscribe(user, 'video');
    },
  },
  //   {
  //     text: "截图",
  //     show: isCamera || isScreen,
  //     onClick: () => {
  //       let dataURL = "";
  //       if (isCamera) dataURL = user.videoTrack.getCurrentFrameData();
  //       if (isScreen) dataURL = user.auxiliaryTrack.getCurrentFrameData();
  //       downloadFileByBase64(dataURL, user.userId);
  //     },
  //   },
  //   {
  //     text: "切大流",
  //     show: !isLocal && isCamera && streamType === "low",
  //     onClick: () => {
  //       client.setRemoteVideoStreamType(user.userId, "high").then(() => {
  //         setStreamType("high");
  //       });
  //     },
  //   },
  //   {
  //     text: "切小流",
  //     show: !isLocal && isCamera && streamType === "high",
  //     onClick: () => {
  //       client.setRemoteVideoStreamType(user.userId, "low").then(() => {
  //         setStreamType("low");
  //       });
  //     },
  //   },
];
</script>

<template>
  <Popover placement="bottom">
    <template #content>
      <Button v-for="btn in btns" :key="btn.text">{{ btn.text }}</Button>
    </template>
    <EllipsisOutlined />
  </Popover>
</template>
