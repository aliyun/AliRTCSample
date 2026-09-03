<template>
  <Row class="previewWrap">
    <Col ref="refContainer" id="refContainer" :class="!deviceInfo.cameraEnable ? 'avatar' : 'camera'">
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
import { getLocalStorageMgr } from '~/utils/LocalStorageMgr';

const refContainer = ref(null);

// 状态管理
const cameraTrack = ref(null);

const globalFlag = useGlobalFlag();
const { userName } = useCurrentUserInfo();
const deviceInfo = useDeviceInfo();
const channelInfo = useChannelInfo();

// 设备操作
const { operateCamera, operateMic, openCamera, openMic } =
  useDevice('pre');
onMounted(async () => {
  // if (globalFlag.isMobile) {
  //   return
  // };
  const previewCamOnOff = getLocalStorageMgr().getLocalStoredData().previewCamOnOff
  if (previewCamOnOff === 'on') {
    const track = await openCamera();
    cameraTrack.value = track;
    if (!globalFlag.joined) {
      track?.play(refContainer.value.$el, { fit: 'cover', mirror: true });
    }
  }
});

const onClickCamera = async () => {
  if (!channelInfo.cameraTrack) {
    getLocalStorageMgr().updateLocalStoredData({ previewCamOnOff: 'on' });
    const track = await openCamera()
    cameraTrack.value = track;
    if (!globalFlag.joined) {
      track?.play(refContainer.value.$el, { fit: 'cover', mirror: true });
    }
  } else {
    getLocalStorageMgr().updateLocalStoredData({ previewCamOnOff: 'off' });
    operateCamera();
  }
};

const onClickMic = () => {
  if (!channelInfo.micTrack) {
    getLocalStorageMgr().updateLocalStoredData({ previewMicOnOff: 'on' });
    openMic();
  } else {
    getLocalStorageMgr().updateLocalStoredData({ previewMicOnOff: 'off' });
    operateMic();
  }
};
</script>

<style lang="less" scoped>
@import url('../index.module.less');
</style>
