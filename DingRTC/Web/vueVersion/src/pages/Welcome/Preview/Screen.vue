<script setup lang="ts">
import { Popover } from 'ant-design-vue';
import { ref } from 'vue';

import { UpOutlined, DownOutlined, FundProjectionScreenOutlined } from '@ant-design/icons-vue';
import { useDevice } from '@src/hooks/useDevices';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';

import ScreenPanel from './ScreenPanel.vue';

const { operateScreen } = useDevice();
const screenSelectorOpen = ref(false);

const localInfoStore = useLocalChannelInfoStore();
const handleScreenOpenChange = () => {
  screenSelectorOpen.value = !screenSelectorOpen.value;
};
</script>

<template>
  <div class="iconWrapper">
    <div class="iconWrapperInner" @click="operateScreen()">
      <FundProjectionScreenOutlined
        :style="{
          color: localInfoStore.screenTrack?.enabled ? 'red' : 'black',
        }"
      />
    </div>
    <div class="gapLine"></div>
    <div class="iconWrapperInner">
      <Popover trigger="click">
        <template #content> <ScreenPanel /> </template>
        <div v-if="screenSelectorOpen" class="iconWrapperInner" @click="handleScreenOpenChange()">
          <DownOutlined />
        </div>
        <div v-else class="iconWrapperInner" @click="handleScreenOpenChange()">
          <UpOutlined />
        </div>
      </Popover>
    </div>
  </div>
</template>
