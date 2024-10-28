<script setup lang="ts">
import { LocalVideoTrack } from 'dingrtc';

import { useDevice } from '@src/hooks/useDevices';
import { useLocalChannel } from '@src/hooks/useLocalChannel';
import { useDeviceInfoStore } from '@src/store';

import VideoEncodeInput1 from './VideoEncorderInputer1.vue';

const { publishedTracks, screenTrack, unPublish, publish } = useLocalChannel();
const { operateScreen } = useDevice('pre');

const isScreenPublish = publishedTracks?.includes(screenTrack as LocalVideoTrack) || false;

const publishScreen = () => {
  if (isScreenPublish) {
    unPublish([screenTrack as LocalVideoTrack]);
  } else if (screenTrack) {
    publish([screenTrack as LocalVideoTrack]);
  } else {
    operateScreen();
  }
};

const deviceInfoStore = useDeviceInfoStore();
</script>
<template>
  <div style="height: 300px; width: 300px">
    <VideoEncodeInput1
      :isPublished="isScreenPublish"
      :dimension="deviceInfoStore.screenDimension"
      :frameRate="deviceInfoStore.screenFrameRate"
      :publish="publishScreen"
      :unPublish="publishScreen"
      type="screen"
    ></VideoEncodeInput1>
  </div>
</template>
