<script setup lang="ts">
import { Popover } from 'ant-design-vue';
import { ref } from 'vue';

import { VideoCameraOutlined, UpOutlined, DownOutlined } from '@ant-design/icons-vue';
import { useDevice } from '@src/hooks/useDevices';

import CameraPanel from './CameraPanel.vue';

const { operateCamera, cameraEnabled } = useDevice('pre');
const cameraSelectorOpen = ref(false);
const handleCameraSelectorOpenChange = (open: boolean) => {
  cameraSelectorOpen.value = open;
};
</script>

<template>
  <div class="iconWrapper">
    <div class="iconWrapperInner" @click="operateCamera()">
      <VideoCameraOutlined
        :style="{
          color: cameraEnabled ? 'red' : 'black',
        }"
      />
    </div>
    <div class="gapLine"></div>
    <Popover trigger="click" @open-change="handleCameraSelectorOpenChange">
      <template #content>
        <CameraPanel />
      </template>
      <div v-if="!cameraSelectorOpen" class="iconWrapperInner">
        <DownOutlined />
      </div>
      <div v-else class="iconWrapperInner">
        <UpOutlined />
      </div>
    </Popover>
  </div>
</template>
