<template>
  <div class="annotation-wrapper">
    <div class="annotation" ref="wrapDomRef">
      <div class="annotation-container" ref="domRef">
        <Toolbar :whiteboard="annotation" />
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { ref, onMounted, onBeforeUnmount, useTemplateRef, computed, toRaw } from 'vue';
import Toolbar from '../../../components/Whiteboard/WBToolbar.vue';
import { useChannelInfo } from '~/store';
import { RtcWhiteboard } from '@dingrtc/whiteboard';

const wrapDomRef = useTemplateRef<HTMLDivElement>('wrapDomRef');
const domRef = useTemplateRef<HTMLDivElement>('domRef');

const channelInfo = useChannelInfo();
// @ts-ignore
window.channelInfo = channelInfo

const activeDocId = ref(channelInfo.annotation?.sessionId);
const config = { width: 1280.0, height: 720.0, limit: true };

function onDocChanged() {
  activeDocId.value = channelInfo.annotation.activeDocId;
}

const annotation = computed(() => toRaw(channelInfo.annotation) as RtcWhiteboard)


function updateWbDomSize() {
  if (
    wrapDomRef.value &&
    domRef.value &&
    wrapDomRef.value.clientWidth !== 0 &&
    wrapDomRef.value.clientHeight !== 0
  ) {
    const wrapDomWidth = wrapDomRef.value.clientWidth;
    const wrapDomHeight = wrapDomRef.value.clientHeight;
    const defaultRadio = config.width / config.height;
    if (wrapDomWidth / wrapDomHeight <= defaultRadio) {
      domRef.value.style.width = '100%';
      domRef.value.style.height = `${wrapDomWidth / defaultRadio}px`;
    } else if (wrapDomWidth / wrapDomHeight > defaultRadio) {
      domRef.value.style.height = '100%';
      domRef.value.style.width = `${wrapDomHeight * defaultRadio}px`;
    }
    channelInfo.annotation.updateCanvasSize(false);
    channelInfo.annotation.center();
  }
}

onMounted(() => {
  const { width, height, limit } = config;
  channelInfo.annotation.initVision(width, height, limit);
  channelInfo.annotation.open(domRef.value!);
  channelInfo.annotation.setRoleType('Admin');
  updateWbDomSize();
  setTimeout(() => {
    channelInfo.annotation.updateCanvasSize();
    channelInfo.annotation.center();
    updateWbDomSize();
  }, 1000);
  window.addEventListener('resize', updateWbDomSize);
  channelInfo.annotation.on('doc-switched', onDocChanged);
});

onBeforeUnmount(() => {
  window.removeEventListener('resize', updateWbDomSize);
  channelInfo.annotation?.off('doc-switched', onDocChanged);
  channelInfo.annotation?.close();
});
</script>

<style lang="less" scoped>
.annotation-wrapper {
  flex: 1;
  padding: 0;
  z-index: 1;
  position: absolute;
  width: 100%;
  height: 100%;
}

.annotation {
  width: 100%;
  height: 100%;
  display: flex;
  background-color: transparent;
  position: absolute;
  justify-content: center;
  align-items: center;

  .annotation-container {
    width: 100%;
    height: 100%;
    position: relative;
    background-color: transparent;
    overflow: hidden;
  }
}
</style>
