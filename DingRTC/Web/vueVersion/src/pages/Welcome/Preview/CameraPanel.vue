<script setup lang="ts">
import DeviceSelector from '@src/components/DeviceSelector.vue';
import { useDeviceInfoStore } from '@src/store';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { print } from '@src/utils/tools';

// import VideoEncoderInput from './VideoEncorderInputer.vue';

const deviceInfoStore = useDeviceInfoStore();
const localChannelStore = useLocalChannelInfoStore();
const updateCamera = (cameraId: string) => {
  deviceInfoStore.$patch({
    cameraId,
  });
  print(`camera changeTo ${cameraId}`);
  localChannelStore.cameraTrack?.setDevice(cameraId);
};
</script>

<template>
  <div class="panelWrapper">
    <DeviceSelector
      title="请选择摄像头"
      :list="deviceInfoStore.cameraList"
      type="cameraId"
      :updateDevice="updateCamera"
    />
    <!-- <div class="encoderSelector"><VideoEncoderInput /></div> -->
  </div>
</template>

<style scoped>
.panelWrapper {
  width: 300px;
}

.deviceItem {
  height: 28px;
  display: flex;
  width: 100%;
  justify-content: left;
  align-items: center;
  :hover {
    cursor: pointer;
  }
}
.selected {
  background-color: #f0f0f0;
}
.deviceName {
  flex: 1;
}
.selectedItem {
  width: 28px;
  height: 28px;
  display: flex;
  justify-content: center;
  align-items: center;
}
.selectorTitle {
  background-color: azure;
}
.encoderSelector {
  display: flex;
  flex-direction: column;
  width: 100%;
}
</style>
