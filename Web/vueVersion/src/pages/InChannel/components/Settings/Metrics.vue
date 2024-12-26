<template>
  <Row>
    <Row class="qualityItem">
      <Row>网络</Row>
      <Row>
        <Col v-for="(item, idx) in networkItems" :key="`${item.label}${idx}`" :span="8">
          <Row class="qualityLabel">
            <Col :span="8">
              {{ item.label }}
            </Col>
            <Col :span="14">
              <span v-if="!item.down">&uarr; {{ item.value }}</span>
              <span v-else>&darr; {{ item.value }}</span>
            </Col>
          </Row>
        </Col>
      </Row>
    </Row>
    <Row class="qualityItem">
      <Row>音频</Row>
      <Row>
        <Col v-for="(item, idx) in audioItems" :key="`${item.label}${idx}`" :span="8">
          <Row class="qualityLabel">
            <Col :span="8">
              {{ item.label }}
            </Col>
            <Col :span="14">
              <span v-if="!item.down">&uarr; {{ item.value }}</span>
              <span v-else>&darr; {{ item.value }}</span>
            </Col>
          </Row>
        </Col>
      </Row>
    </Row>
    <Row class="qualityItem">
      <Row>视频</Row>
      <Row>
        <Col v-for="(item, idx) in videoItems" :key="`${item.label}${idx}`" :span="8">
          <Row class="qualityLabel">
            <Col :span="8">
              {{ item.label }}
            </Col>
            <Col :span="14">
              <span v-if="!item.down">&uarr; {{ item.value }}</span>
              <span v-else>&darr; {{ item.value }}</span>
            </Col>
          </Row>
        </Col>
      </Row>
    </Row>
    <Row class="qualityItem">
      <Row>屏幕分享</Row>
      <Row>
        <Col v-for="(item, idx) in screenItems" :key="`${item.label}${idx}`" :span="8">
          <Row class="qualityLabel">
            <Col :span="8">
              {{ item.label }}
            </Col>
            <Col :span="14">
              <span v-if="!item.down">&uarr; {{ item.value }}</span>
              <span v-else>&darr; {{ item.value }}</span>
            </Col>
          </Row>
        </Col>
      </Row>
    </Row>
  </Row>
</template>
<script setup lang="ts">
import { Col, Row } from 'ant-design-vue';
import { computed } from 'vue';
import { useChannelInfo } from '~/store';

const channelInfo = useChannelInfo();

const networkItems = computed(() => [
  {
    label: '带宽',
    value: channelInfo.rtcStats.localBitrate
      ? `${Math.round(channelInfo.rtcStats.localBitrate / 1000)} kbps`
      : '--',
  },
  // {
  //   label: '丢包率',
  //   value: `${channelInfo.rtcStats.loss} %`,
  // },

  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteBitrate
      ? `${Math.round(channelInfo.rtcStats.remoteBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '延迟',
    value: channelInfo.rtcStats.rtt ? `${channelInfo.rtcStats.rtt} ms` : '--',
  },
]);
const audioItems = computed(() => [
  {
    label: '码率',
    value: channelInfo.rtcStats.localAudioBitrate
      ? `${Math.round(channelInfo.rtcStats.localAudioBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteAudioBitrate
      ? `${Math.round(channelInfo.rtcStats.remoteAudioBitrate / 1000)} kbps`
      : '--',
  },
]);
const videoItems = computed(() => [
  {
    label: '分辨率',
    value: channelInfo.rtcStats.localCameraResolution
      ? `${channelInfo.rtcStats.localCameraResolution.width}*${channelInfo.rtcStats.localCameraResolution.height}`
      : '--',
  },
  {
    label: '码率',
    value: channelInfo.rtcStats.localCameraBitrate
      ? `${Math.round(channelInfo.rtcStats.localCameraBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '帧率',
    value: channelInfo.rtcStats.localCameraFPS
      ? `${channelInfo.rtcStats.localCameraFPS} fps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteCameraResolution
      ? `${channelInfo.rtcStats.remoteCameraResolution.width}*${channelInfo.rtcStats.remoteCameraResolution.height}`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteCameraBitrate
      ? `${Math.round(channelInfo.rtcStats.remoteCameraBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteCameraFPS
      ? `${channelInfo.rtcStats.remoteCameraFPS} fps`
      : '--',
  },
]);
const screenItems = computed(() => [
  {
    label: '分辨率',
    value: channelInfo.rtcStats.localScreenResolution
      ? `${channelInfo.rtcStats.localScreenResolution.width}*${channelInfo.rtcStats.localScreenResolution.height}`
      : '--',
  },
  {
    label: '码率',
    value: channelInfo.rtcStats.localScreenBitrate
      ? `${Math.round(channelInfo.rtcStats.localScreenBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '帧率',
    value: channelInfo.rtcStats.localScreenFPS
      ? `${channelInfo.rtcStats.localScreenFPS} fps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteScreenResolution
      ? `${channelInfo.rtcStats.remoteScreenResolution.width}*${channelInfo.rtcStats.remoteScreenResolution.height}`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteScreenBitrate
      ? `${Math.round(channelInfo.rtcStats.remoteScreenBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.remoteScreenFPS
      ? `${channelInfo.rtcStats.remoteScreenFPS} fps`
      : '--',
  },
]);
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
