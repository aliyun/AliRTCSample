<script lang="ts" setup>
import { LocalVideoTrack, RemoteVideoTrack } from 'dingrtc';
import { ref, computed, watchEffect } from 'vue';
import { Col, Row } from 'ant-design-vue';
import Icon from '../Icon';
import { useGlobalFlag } from '~/store';

interface IProps {
  track?: LocalVideoTrack | RemoteVideoTrack;
  container?: HTMLDivElement;
}

const props = defineProps<IProps>();
const globalFlag = useGlobalFlag();

const played = ref(false);
const { track } = props;

const resumePlay = () => {
  track?.play?.(props.container!, { fit: 'cover' });
  played.value = true;
};

watchEffect(() => {
  if (!track) played.value = false;
});
const needResume = computed(() => {
  return false
  if (!track || !(globalFlag.isIOS && globalFlag.isWeixin) || played.value) return false;
  return track;
});
</script>
<template>
  <Row v-if="needResume" class="needResume" @click="resumePlay">
    <Col>
      <Icon type="icon-XDS_VideoFill" />
    </Col>
    <Col> iOS微信内需要点击播放 </Col>
  </Row>
</template>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
