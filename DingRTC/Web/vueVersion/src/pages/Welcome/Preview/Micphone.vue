<script setup lang="ts">
import { Popover } from 'ant-design-vue';
import { ref } from 'vue';

import { AudioOutlined, UpOutlined, DownOutlined } from '@ant-design/icons-vue';
import { useDevice } from '@src/hooks/useDevices';

import MicPanel from './MicPanel.vue';

const { operateMic, micEnabled } = useDevice('pre');
const micSelectorOpen = ref(false);
const handleMicSelectorOpenChange = (open: boolean) => {
  micSelectorOpen.value = open;
};
</script>

<template>
  <div class="iconWrapper">
    <div class="iconWrapperInner" @click="operateMic()">
      <AudioOutlined :style="{ color: micEnabled ? 'red' : 'black' }" />
    </div>
    <div class="gapLine"></div>
    <Popover trigger="click" @open-change="handleMicSelectorOpenChange">
      <template #content>
        <MicPanel />
      </template>
      <div v-if="!micSelectorOpen" class="iconWrapperInner">
        <DownOutlined />
      </div>
      <div v-else class="iconWrapperInner">
        <UpOutlined />
      </div>
    </Popover>
  </div>
</template>
<style>
.iconWrapper {
  display: flex;
  justify-content: center;
  align-items: center;
  border-radius: 8px;
  overflow: hidden;
  height: 32px;
  width: 80px;
  background-color: #fff;
}
.iconWrapperInner {
  height: 100%;
  width: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
}
.gapLine {
  height: 18px;
  width: 0;
  border: 1px solid hsla(210, 7%, 53%, 0.18);
}
</style>
