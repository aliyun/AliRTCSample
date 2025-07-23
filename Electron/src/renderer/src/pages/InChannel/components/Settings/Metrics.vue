<template>
  <Row class="hardwareInfos" justify="space-between">
    <Col :span="11" class="qualityItem">
      <Row justify="space-between">
        <span>CPU</span>
        <span
          >{{ channelInfo.rtcStats.cpuLogicCores }}核
          {{ channelInfo.rtcStats.cpuFrequencyMhz / 1000 }}Ghz</span
        >
      </Row>
      <Row justify="space-between" class="qualityLabel">
        <Col><i class="blueDot" /> demo应用 </Col>
        <Col> {{ channelInfo.rtcStats.cpuUsedByApp || 0 }} % </Col>
      </Row>
      <Row justify="space-between" class="qualityLabel">
        <Col><i class="redDot" /> 其他 </Col>
        <Col> {{ channelInfo.rtcStats.cpuUsedByOtherApps || 0 }} % </Col>
      </Row>
      <Row>
        <Progress
          :percent="channelInfo.rtcStats.cpuUsedByApp + channelInfo.rtcStats.cpuUsedByOtherApps"
          :show-info="false"
          stroke-color="#FF9200"
          :success="{ percent: channelInfo.rtcStats.cpuUsedByApp, strokeColor: '#007FFF' }"
        />
      </Row>
    </Col>
    <Col :span="11" class="qualityItem">
      <Row justify="space-between">
        <span>内存</span>
        <span>{{ transMbToGb(channelInfo.rtcStats.memoryTotalMb) }}</span>
      </Row>
      <Row justify="space-between" class="qualityLabel">
        <Col><i class="blueDot" /> demo应用 </Col>
        <Col>
          {{ transMbToGb(channelInfo.rtcStats.memoryUsedByApp) }}
        </Col>
      </Row>
      <Row justify="space-between" class="qualityLabel">
        <Col><i class="redDot" /> 其他 </Col>
        <Col>
          {{ transMbToGb(channelInfo.rtcStats.memoryUsedByOtherApps) }}
        </Col>
      </Row>
      <Row>
        <Progress
          :percent="
            (channelInfo.rtcStats.memoryUsedByApp + channelInfo.rtcStats.memoryUsedByOtherApps) /
            channelInfo.rtcStats.memoryTotalMb * 100
          "
          :show-info="false"
          stroke-color="#FF9200"
          :success="{ percent: (channelInfo.rtcStats.memoryUsedByApp / channelInfo.rtcStats.memoryTotalMb) * 100, strokeColor: '#007FFF' }"
        />
      </Row>
    </Col>

    <Row class="qualityItem">
      <Row>网络</Row>
      <Row>
        <Col v-for="(item, idx) in networkItems" :key="`${item.label}${idx}`" :span="8">
          <Row class="qualityLabel">
            <Col :span="8">
              {{ item.label }}
            </Col>
            <Col :span="14">
              <span v-if="item.down">&darr; {{ item.value }}</span>
              <span v-else-if="item.up">&uarr; {{ item.value }}</span>
              <span v-else>{{ item.value }}</span>
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
              <span v-if="item.down">&darr; {{ item.value }}</span>
              <span v-else-if="item.up">&uarr; {{ item.value }}</span>
              <span v-else>{{ item.value }}</span>
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
              <span v-if="item.down">&darr; {{ item.value }}</span>
              <span v-else-if="item.up">&uarr; {{ item.value }}</span>
              <span v-else>{{ item.value }}</span>
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
              <span v-if="item.down">&darr; {{ item.value }}</span>
              <span v-else-if="item.up">&uarr; {{ item.value }}</span>
              <span v-else>{{ item.value }}</span>
            </Col>
          </Row>
        </Col>
      </Row>
    </Row>
  </Row>
</template>
<script setup lang="ts">
import { Col, Row, Progress } from 'ant-design-vue';
import { computed } from 'vue';
import { useChannelInfo } from '~/store';
import { transMbToGb } from '~/utils/tools';

const channelInfo = useChannelInfo();

const networkItems = computed(() => [
  {
    label: '带宽',
    up: true,
    value: channelInfo.rtcStats.sendBitrate
      ? `${Math.round(channelInfo.rtcStats.sendBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '丢包率',
    value: `${channelInfo.rtcStats.lossRate} %`,
  },
  {
    label: '网络类型',
    value: channelInfo.rtcStats.networkType || '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.recvBitrate
      ? `${Math.round(channelInfo.rtcStats.recvBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '延迟',
    value: channelInfo.rtcStats.rttMs ? `${channelInfo.rtcStats.rttMs} ms` : '--',
  },
]);
const audioItems = computed(() => [
  {
    label: '码率',
    up: true,
    value: channelInfo.rtcStats.audioSendBitrate
      ? `${Math.round(channelInfo.rtcStats.audioSendBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '麦克风',
    value: `${Math.round(90 - (channelInfo.rtcStats.audioSendVolume || 90))} dB`,
  },
  {
    label: '扬声器',
    value: `${Math.round(90 - (channelInfo.rtcStats.audioRecvVolume || 90))} dB`,
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.audioRecvBitrate
      ? `${Math.round(channelInfo.rtcStats.audioRecvBitrate / 1000)} kbps`
      : '--',
  },
]);
const videoItems = computed(() => [
  {
    label: '分辨率',
    up: true,
    value: channelInfo.rtcStats.cameraSendWidth
      ? `${channelInfo.rtcStats.cameraSendWidth}*${channelInfo.rtcStats.cameraSendHeight}`
      : '--',
  },
  {
    label: '码率',
    up: true,
    value: channelInfo.rtcStats.cameraSendBitrate
      ? `${Math.round(channelInfo.rtcStats.cameraSendBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '帧率',
    up: true,
    value: channelInfo.rtcStats.cameraSendFps ? `${channelInfo.rtcStats.cameraSendFps} fps` : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.cameraRecvWidth
      ? `${channelInfo.rtcStats.cameraRecvWidth}*${channelInfo.rtcStats.cameraRecvHeight}`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.cameraRecvBitrate
      ? `${Math.round(channelInfo.rtcStats.cameraRecvBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.cameraRecvFps ? `${channelInfo.rtcStats.cameraRecvFps} fps` : '--',
  },
]);
const screenItems = computed(() => [
  {
    label: '分辨率',
    up: true,
    value: channelInfo.rtcStats.screenshareSendWidth
      ? `${channelInfo.rtcStats.screenshareSendWidth}*${channelInfo.rtcStats.screenshareSendHeight}`
      : '--',
  },
  {
    label: '码率',
    up: true,
    value: channelInfo.rtcStats.screenshareSendBitrate
      ? `${Math.round(channelInfo.rtcStats.screenshareSendBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '帧率',
    up: true,
    value: channelInfo.rtcStats.screenshareSendFps
      ? `${channelInfo.rtcStats.screenshareSendFps} fps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.screenshareRecvWidth
      ? `${channelInfo.rtcStats.screenshareRecvWidth}*${channelInfo.rtcStats.screenshareRecvHeight}`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.screenshareRecvBitrate
      ? `${Math.round(channelInfo.rtcStats.screenshareRecvBitrate / 1000)} kbps`
      : '--',
  },
  {
    label: '',
    down: true,
    value: channelInfo.rtcStats.screenshareRecvFps
      ? `${channelInfo.rtcStats.screenshareRecvFps} fps`
      : '--',
  },
]);
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
