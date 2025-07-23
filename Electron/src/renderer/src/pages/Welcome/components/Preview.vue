<template>
  <Row class="previewWrap">
    <Col ref="refContainer" :class="!deviceInfo.cameraEnable ? 'avatar' : 'camera'">
      <Avatar size="large">{{ userName }}</Avatar>
    </Col>
    <Row class="devices">
      <Col class="deviceColumn">
        <Camera :click="operateCamera" />
      </Col>
      <Divider type="vertical" />
      <Col class="deviceColumn">
        <Mic :click="operateMic" />
      </Col>
    </Row>
  </Row>

</template>

<script setup lang="ts">
import { Row, Col, Divider, Avatar } from 'ant-design-vue';
import { ref, onMounted, toRaw } from 'vue';
import { Mic, Camera } from '~/components/Device';
import { useDevice } from '~/hooks/device';
import { useCurrentUserInfo, useDeviceInfo } from '~/store';
const refContainer = ref(null);

// 状态管理
const cameraTrack = ref(null);

const { userName } = useCurrentUserInfo();
const deviceInfo = useDeviceInfo();

// 设备操作
const { openMicAndCameraSameTime, operateCamera, operateMic } = useDevice('pre');
onMounted(async () => {
  const track = await openMicAndCameraSameTime();
  cameraTrack.value = track;
  if (cameraTrack.value) {
    toRaw(cameraTrack.value).play(refContainer.value.$el, { fit: 'cover' });
  }
});
</script>

<style lang="less" scoped>
@import url('../index.module.less');
</style>
