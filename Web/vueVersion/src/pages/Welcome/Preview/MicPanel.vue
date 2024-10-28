<script setup lang="ts">
import DeviceSelector from '@src/components/DeviceSelector.vue';
import { useDeviceInfoStore } from '@src/store';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';

const deviceInfoStore = useDeviceInfoStore();
const localChannelStore = useLocalChannelInfoStore();
const updateMic = (micId: string) => {
  deviceInfoStore.$patch({
    micId,
  });
  localChannelStore.micTrack?.setDevice(micId);
};
</script>

<template>
  <div class="panelWrapper">
    <DeviceSelector
      title="请选择麦克风"
      :list="deviceInfoStore.micList"
      type="micId"
      :updateDevice="updateMic"
    />
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
</style>
