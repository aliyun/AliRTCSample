<template>
  <Row>
    <Form :model="formData" style="width: 100%" label-align="left">
      <Divider orientation="left" plain>
        <Text>视频编码设置</Text>
      </Divider>
      <Form.Item label="分辨率" name="dimension">
        <Select v-model:value="formData.dimension" :options="videoDimensions.map((item)=> ({ label: item, value: item }))" />
      </Form.Item>
      <Row justify="space-between">
      <Form.Item label="帧&nbsp&nbsp&nbsp&nbsp率" name="frameRate">
        <Select v-model:value="formData.frameRate" :options="frameRates.map((item)=> ({ label: item, value: item }))" />
      </Form.Item>
      <Form.Item label="优化模式" name="optimizationMode">
        <Select
          @change="onModeChange"
          v-model:value="formData.optimizationMode"
          :options="optionzationModeList"
        />
      </Form.Item>
      </Row>
      <Form.Item label="最大码率" name="maxBitrate">
        <InputNumber v-model:value="formData.maxBitrate" placeholder="默认用分辨率、帧率计算" addon-after="Kbps" />
      </Form.Item>
      <Button class="apply" @click="updateEncoder">
        应用
      </Button>
    </Form>
  </Row>
</template>
<script setup lang="ts">
import { LocalVideoTrack } from 'dingrtc';
import {
  Row,
  Typography,
  Divider,
  Form,
  Select,
  InputNumber,
  Button,
  message,
} from 'ant-design-vue';
import { reactive } from 'vue';
import { useChannel } from '~/hooks/channel';
import { useDeviceInfo, useChannelInfo } from '~/store';
import { videoDimensions, frameRates, optionzationModeList,  } from '~/constants';

const props = defineProps(['close']);
const deviceInfo = useDeviceInfo();
const channelInfo = useChannelInfo();
const { Text } = Typography;

const formData = reactive({
  dimension: deviceInfo.screenDimension,
  screenDimension: deviceInfo.screenDimension,
  frameRate: deviceInfo.screenFrameRate,
  maxBitrate: deviceInfo.screenMaxBitrate,
  optimizationMode: deviceInfo.screenOptimization,
})

const { close } = props;
const { publish, unpublish } = useChannel();

const onModeChange = (value) => {
  if (channelInfo.cameraTrack) {
    deviceInfo.$patch({ screenOptimization: value })
  }
}

const updateEncoder = () => {
  const { dimension, frameRate, maxBitrate, optimizationMode } = formData;
  const isScreenPublish = channelInfo.publishedTracks.has(channelInfo.screenTrack?.getTrackId());

  const fn = () => {
    if (!channelInfo.screenTrack) {
      message.info('请先创建共享轨道');
      return;
    }
    channelInfo.screenTrack
      .setEncoderConfiguration({
        frameRate,
        dimension,
        maxBitrate,
        optimizationMode,
      })
      .then(() => {
        deviceInfo.$patch({
          screenMaxBitrate: channelInfo.screenTrack.getMaxBitrate(),
        });
        publish([channelInfo.screenTrack as LocalVideoTrack]).then(() => {
          message.info('调整编码参数并重新发布成功');
          close();
        });
      });
  };
  if (isScreenPublish) {
    unpublish([channelInfo.screenTrack as LocalVideoTrack]).then(() => {
      fn();
    });
  } else {
    fn();
  }
  deviceInfo.$patch({
    screenDimension: dimension,
    screenMaxBitrate: maxBitrate,
    screenFrameRate: frameRate,
    screenOptimization: optimizationMode,
  });
}
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>