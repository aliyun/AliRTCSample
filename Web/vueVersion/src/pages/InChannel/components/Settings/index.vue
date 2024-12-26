<template>
  <Modal
    open
    :footer="null"
    wrap-class-name="settings"
    :width="680"
    @cancel="onCancel"
  >
    <Row style="height: 500">
      <Col :span="7" class="settingMenu">
        <Menu :selected-keys="[activeTab]" :items="menus" @click="onChangeTab" />
      </Col>
      <Col class="settingDetail" :span="17">
        <Title :level="5">
          {{ activeMenu.label }}
          <Text v-if="activeMenu.tip" class="tip" type="secondary">
            ({{ activeMenu.tip }})
          </Text>
        </Title>
        <Divider />
        <General v-if="activeTab === 'general'" />
        <Video v-else-if="activeTab === 'video'" :close="close" />
        <Audio v-else-if="activeTab === 'audio'" />
        <ScreenShare v-else-if="activeTab === 'share'" :close="close" />
        <External v-else-if="activeTab === 'external'" />
        <Metrics v-else-if="activeTab === 'metrics'" />
        <GroupList v-else-if="activeTab === 'group'" /> 
      </Col>
    </Row>
  </Modal>
</template>
<script lang="ts" setup>
import { Col, Modal, Row, Typography, Menu, Divider, MenuProps } from 'ant-design-vue';
import Icon from '~/components/Icon';
import { useChannel } from '~/hooks/channel';
import General from './General.vue';
import Audio from './Audio.vue';
import Video from './Video.vue';
import ScreenShare from './ScreenShare.vue';
import External from './External.vue';
import Metrics from './Metrics.vue';
import GroupList from './Group/List.vue';
import { computed, ref, h } from 'vue';
import { useChannelInfo } from '~/store';

const { Title, Text } = Typography;

const props = defineProps(['close']);

const menus = [
  {
    label: '通用',
    icon: h(Icon, { type: "icon-icon_x_Settings" }),
    key: 'general',
  },
  {
    label: '视频',
    icon: h(Icon, { type: "icon-XDS_FrameMeetingLine" }),
    key: 'video',
  },
  {
    label: '音频',
    icon: h(Icon, { type: "icon-XDS_Mute2" }),
    key: 'audio',
  },
  {
    label: '共享屏幕',
    icon: h(Icon, { type: "icon-XDS_share_screen1" }),
    key: 'share',
  },
  {
    label: '分组讨论',
    icon: h(Icon, { type: "icon-XDS_Confm" }),
    key: 'group',
  },
  {
    label: '外部源',
    icon: h(Icon, { type: "icon-XDS_Documents" }),
    key: 'external',
    tip: '仅在当前窗口打开时生效',
  },
  {
    label: '质量监测',
    icon: h(Icon, { type: "icon-data_usage_20" }),
    key: 'metrics',
  },
];

const activeTab = ref('general');
const channelInfo = useChannelInfo();
const { publish, unpublish } = useChannel();
const publishStateSnapshot = ref({
  camera: channelInfo.publishedTracks.has(channelInfo.cameraTrack.getTrackId()),
  mic: channelInfo.publishedTracks.has(channelInfo.micTrack.getTrackId()),
});
const onUnloadExternal = async () => {
  const tracksToUnpublish: any[] = [
    channelInfo.customAudioTrack,
    channelInfo.customVideoTrack,
  ].filter((item) => item && channelInfo.publishedTracks.has(item.getTrackId()));
  if (tracksToUnpublish.length) {
    await unpublish(tracksToUnpublish);
  }
  const tracksToPublish: any[] = [];
  if (
    channelInfo.cameraTrack &&
    !channelInfo.publishedTracks.has(channelInfo.cameraTrack.getTrackId()) &&
    publishStateSnapshot.value.camera
  ) {
    tracksToPublish.push(channelInfo.cameraTrack);
  }
  if (
    channelInfo.micTrack &&
    !channelInfo.publishedTracks.has(channelInfo.micTrack.getTrackId()) &&
    publishStateSnapshot.value.mic
  ) {
    tracksToPublish.push(channelInfo.micTrack);
  }
  if (tracksToPublish.length) {
    await publish(tracksToPublish);
  }
};
const activeMenu = computed(() => {
  return menus.find((item) => item.key === activeTab.value);
});

const onChangeTab: MenuProps['onClick'] = (e) => {
  if (activeTab.value === 'external') {
    onUnloadExternal();
  }
  activeTab.value = e.key as string;
};

const onCancel = () => {
  if (activeTab.value === 'external') {
    onUnloadExternal();
  }
  props.close();
};

</script>
<style lang="less">
@import url('./index.module.less');
</style>
