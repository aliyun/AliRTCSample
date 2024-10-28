<script setup lang="ts">
import { CheckOutlined } from '@ant-design/icons-vue';
import { useDeviceInfoStore } from '@src/store/deviceInfo';

type MediaDeviceKind = 'audioinput' | 'audiooutput' | 'videoinput';

interface Device {
  deviceId: string;
  groupId: string;
  kind: MediaDeviceKind;
  label: string;
}

const props = defineProps<{
  list: Device[];
  type: 'cameraId' | 'micId' | 'speakerId';
  title: string;
  updateDevice: (deviceId: string) => void;
}>();

const deviceInfoStore = useDeviceInfoStore();
</script>

<template>
  <div class="deviceSelector">
    <div class="selectorTitle">{{ props.title }}</div>
    <div
      v-for="device in props.list"
      :class="['deviceItem', device.deviceId === deviceInfoStore[props.type] && 'selected']"
      :key="device.deviceId"
    >
      <div v-if="device.deviceId === deviceInfoStore[props.type]" class="selectedItem">
        <CheckOutlined />
      </div>
      <div v-else class="selectedItem"></div>
      <div @click="props.updateDevice(device.deviceId)" class="deviceName">
        {{ device.label }}
      </div>
    </div>
  </div>
</template>

<style lang="less" scoped>
.deviceSelector {
  display: flex;
  flex-direction: column;
  width: 300px;
}
.selectorTitle {
  color: rgba(23, 26, 29, 0.4);
  font-size: 12px;
  height: 28px;
  line-height: 28px;
}

.deviceItem {
  display: flex;
  width: 100%;
  justify-content: left;
  align-items: center;
  cursor: pointer;
  height: 36px;
  line-height: 36px;
  overflow: hidden;
  position: relative;
  white-space: nowrap;
  background-color: hsla(0, 0%, 100%, 0.8);
  border-radius: 4px;
  &:hover {
    cursor: pointer;
    background-color: #b3b3b4;
  }
}

.selected {
  background-color: #f0f0f0;
}

.selectedItem {
  width: 28px;
  height: 28px;
  display: flex;
  justify-content: center;
  align-items: center;
}

.deviceName {
  flex: 1;
}
</style>
