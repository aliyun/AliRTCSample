<template>
  <Row class="previewWrap">
    <Col ref="refContainer" :class="!deviceInfo.cameraEnable ? 'avatar' : 'camera'">
      <Avatar size="large">{{ userName }}</Avatar>
    </Col>
    <Row class="devices">
      <Col class="deviceColumn">
        <Camera :click="onClickCamera" />
      </Col>
      <Divider type="vertical" />
      <Col class="deviceColumn">
        <Mic :click="onClickMic" />
      </Col>
    </Row>
  </Row>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue';
import { Mic, Camera } from '~/components/Device';
import { Row, Col, Divider, Avatar } from 'ant-design-vue';
import { useDevice } from '~/hooks/device';
import { useChannelInfo, useCurrentUserInfo, useDeviceInfo, useGlobalFlag } from '~/store';

const refContainer = ref(null);

// 状态管理
const cameraTrack = ref(null);

const globalFlag = useGlobalFlag()
const { userName } = useCurrentUserInfo();
const deviceInfo = useDeviceInfo()
const channelInfo = useChannelInfo();

// 设备操作
const { openMicAndCameraSameTime, operateCamera, operateMic, openCamera, openMic } = useDevice('pre');
onMounted(async () => {
  if (globalFlag.isMobile) return;
  const [track] = await openMicAndCameraSameTime();
  cameraTrack.value = track;
  track?.play(refContainer.value.$el, { fit: 'cover' });
});

const onClickCamera = () => {
  if (globalFlag.isMobile && !channelInfo.cameraTrack) {
    openCamera().then((track) => {
      track?.play(refContainer.value.$el, { fit: 'cover' });
    })
  } else {
    operateCamera()
  }
}

const onClickMic = () => {
  if (globalFlag.isMobile && !channelInfo.micTrack) {
    openMic()
  } else {
    operateMic()
  }
}
</script>

<style lang="less" scoped>
@import url('../index.module.less');
</style>