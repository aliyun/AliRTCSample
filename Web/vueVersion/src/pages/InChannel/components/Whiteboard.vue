<template>
  <div class="dingrtc-wb-wrapper">
    <div class="dingrtc-wb" ref="wrapDomRef">
      <div class="whiteboard-container" ref="domRef">
        <Toolbar :whiteboard="whiteboard" />
        <VisionShare :whiteboard="whiteboard" />
        <ToolPagination :activeDocId="activeDocId" :whiteboard="whiteboard" />
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { ref, onMounted, onBeforeUnmount, useTemplateRef, computed } from 'vue';
import Toolbar from '../../../components/Whiteboard/WBToolbar.vue';
import VisionShare from '../../../components/Whiteboard/VisionShare.vue';
import ToolPagination from '../../../components/Whiteboard/Pagination.vue';
import { useChannelInfo } from '~/store';
import { defaultWhiteboardId } from '~/constants/index';
import { RtcWhiteboard } from '@dingrtc/whiteboard';
import { logger } from '~/utils/tools';


const channelInfo = useChannelInfo();
const wrapDomRef = useTemplateRef<HTMLDivElement>('wrapDomRef');
const domRef = useTemplateRef<HTMLDivElement>('domRef');

const activeDocId = ref(defaultWhiteboardId);
const config = { width: 1280.0, height: 720.0, limit: true };

function onDocChanged() {
  activeDocId.value = channelInfo.whiteboard.activeDocId;
}

const whiteboard = computed(() => channelInfo.whiteboard as RtcWhiteboard)

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
    channelInfo.whiteboard.updateCanvasSize(false);
    channelInfo.whiteboard.center();
  }
}

onMounted(async () => {
  const { width, height, limit } = config;
  channelInfo.whiteboard.initVision(width, height, limit);
  try {
    await channelInfo.whiteboard.open(domRef.value!);
  } catch (error) {
    logger.info('wb open error', error);
  }

  updateWbDomSize();
  setTimeout(() => {
    channelInfo.whiteboard.updateCanvasSize();
    channelInfo.whiteboard.center();
    updateWbDomSize();
  }, 1000);
  window.addEventListener('resize', updateWbDomSize);
  channelInfo.whiteboard.on('doc-switched', onDocChanged);
});

onBeforeUnmount(() => {
  window.removeEventListener('resize', updateWbDomSize);
  channelInfo.whiteboard?.off('doc-switched', onDocChanged);
  channelInfo.whiteboard?.close();
});
</script>

<style lang="less" scoped>
.dingrtc-wb-wrapper {
  flex: 1;
  padding: 0 0 60px;
}

.dingrtc-wb {
  width: 100%;
  height: 100%;
  display: flex;
  background-color: #dce1e6;
  position: relative;
  justify-content: center;
  align-items: center;

  .whiteboard-container {
    width: 100%;
    height: 100%;
    position: relative;
    background-color: rgba(255, 255, 255, 1);
    overflow: hidden;
  }
}
</style>
