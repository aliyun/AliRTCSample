<template>
  <Modal open :closable="false" class="shareContainerModal" :width="680" @cancel="close">
    <template #footer>

      <div
        :class="[
          'shareContainerFooter',
          { shareContainerFooterWithBorder: windowSources.length > 12 },
        ]"
      >
        <Checkbox :checkd="shareAudio" @change="onChangeShareAudio">共享电脑音频</Checkbox>
        <Space :size="16" :style="{ marginRight: 12 }">
          <Button @click="close">取消</Button>
          <Button type="primary"  @click="onOk" :disabled="!channelInfo.shareSourceId"> 确认 </Button>
        </Space>
      </div>
    </template>
    <Row class="shareContainer">
      <Row class="shareContainerBody">
        <Space wrap style="margin-bottom: 10px">
          <div
            v-for="(item, idx) in screenSources"
            :class="[
              'shareItem',
              { shareItemSelected: channelInfo.shareSourceId === item.sourceId },
            ]"
            @click="onClick(item)"
            @dblclick="onDoubleClick(item)"
          >
            <div>
              <Tooltip :title="`桌面${idx + 1}`">
                <span class="shareItemTitle">{{ `桌面${idx + 1}` }}</span>
              </Tooltip>
            </div>
            <div :style="{ backgroundImage: `url(${item.thumbImageSrc})` }" />
          </div>
        </Space>
        <Space wrap>
          <div
            v-for="item in windowSources"
            :class="[
              'shareItem',
              { shareItemSelected: channelInfo.shareSourceId === item.sourceId },
            ]"
            @click="onClick(item)"
            @dblclick="onDoubleClick(item)"
          >
            <div>
              <img v-if="item.iconImageSrc" :src="item.iconImageSrc" class="shareItemIcon" />
              <Tooltip :title="item.sourceName">
                <span class="shareItemTitle">{{ item.sourceName }}</span>
              </Tooltip>
            </div>
            <div
              v-if="item.thumbImageSrc"
              :style="{ backgroundImage: `url(${item.thumbImageSrc})` }"
            />
            <div v-else>
              <img :src="item.iconImageSrc" />
            </div>
          </div>
        </Space>
      </Row>
    </Row>
  </Modal>
</template>
<script setup lang="ts">
import { Modal, Row, Space, Button, Tooltip, Checkbox } from 'ant-design-vue';
import { onMounted, ref } from 'vue';
import { useChannelInfo, useDeviceInfo } from '~/store';

import DingRTC, { ScreenCaptureSourceInfo } from 'dingrtc-electron-sdk';
import { useDevice } from '~/hooks/device';

const props = defineProps(['close']);
const channelInfo = useChannelInfo();
const deviceInfo = useDeviceInfo();
const { operateScreen } = useDevice();
const shareAudio = ref(deviceInfo.shareSystemAudio);
const screenSources = ref<ScreenCaptureSourceInfo[]>([]);
const windowSources = ref<ScreenCaptureSourceInfo[]>([]);

const onChangeShareAudio = (e) => {
  shareAudio.value = e.target.checked;
  deviceInfo.shareSystemAudio = e.target.checked
}

onMounted(() => {
  const resources = DingRTC.getScreenCaptureSources(true);
  screenSources.value = resources.filter((item) => item.type === 'screen');
  windowSources.value = resources.filter((item) => item.type === 'window');
});

const onClick = (source: ScreenCaptureSourceInfo) => {
  channelInfo.$patch({
    shareSourceId: source.sourceId,
    shareSourceType: source.type,
  });
};
const onOk = () => {
  operateScreen(channelInfo.shareSourceType, channelInfo.shareSourceId)
  channelInfo.micTrack.enableSystemAudio(shareAudio.value);
  props.close();
}
const onDoubleClick = (source: ScreenCaptureSourceInfo) => {
  channelInfo.$patch({
    shareSourceId: source.sourceId,
    shareSourceType: source.type,
  });
  onOk()
};
</script>
<style lang="less">
@import url('../index.module.less');
</style>
