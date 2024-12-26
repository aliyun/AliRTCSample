<template>
  <Row class="videoSetting">
    <div class="videoWrapper">
      <video ref="videoSettingRef" loop autoPlay playsInline webkit-playsinline :controls="false" muted />
    </div>
    <Form style="width: 100%" :label-col="{ span: 4 }" label-align="left" :model="formData">
      <Form.Item label="摄像头" name="cameraId">
        <Select
          v-model:value="formData.cameraId"
          :options="deviceInfo.cameraList.map((item) => ({ label: item.label, value: item.deviceId }))"
          @change="onCameraIdChange"
        />
      </Form.Item>
      <Divider orientation="left" plain>
        <Text>视频编码设置</Text>
      </Divider>
      <Form.Item label="分辨率" name="dimension">
        <Select
          v-model:value="formData.dimension"
          :options="videoDimensions.map((item) => ({ label: item, value: item }))"
        />
      </Form.Item>
      <Form.Item label="帧率" name="frameRate">
        <Select
          v-model:value="formData.frameRate"
          :options="frameRates.map((item) => ({ label: item, value: item }))"
        />
      </Form.Item>
      <Form.Item label="优化模式" name="optionzationMode">
        <Select
          @change="onModeChange"
          v-model:value="formData.optimizationMode"
          :options="optionzationModeList"
        />
      </Form.Item>
      <Form.Item label="最大码率" name="maxBitrate">
        <InputNumber
          v-model:value="formData.maxBitrate"
          placeholder="默认用分辨率、帧率计算"
          addon-after="Kbps"
        />
      </Form.Item>
      <Button class="apply" @click="updateEncoder"> 应用 </Button>
    </Form>
  </Row>
</template>
<script lang="ts" setup>
import { CameraVideoTrack } from 'dingrtc';
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
import { reactive, ref, watchEffect } from 'vue';
import { useChannel } from '~/hooks/channel';
import { useDeviceInfo, useChannelInfo } from '~/store';
import { videoDimensions, frameRates, optionzationModeList,  } from '~/constants';

const { Text } = Typography;

const props = defineProps(['close']);
const { close } = props;
const videoSettingRef = ref<HTMLVideoElement>();
const deviceInfo = useDeviceInfo();

const formData = reactive({
  cameraId: deviceInfo.cameraId,
  maxBitrate: deviceInfo.cameraMaxBitrate,
  dimension: deviceInfo.cameraDimension,
  frameRate: deviceInfo.cameraFrameRate,
  optimizationMode: deviceInfo.cameraOptimization,
});
const channelInfo = useChannelInfo();
const { publish, unpublish } = useChannel();

const onModeChange = (value) => {
  if (channelInfo.cameraTrack) {
    deviceInfo.$patch({ cameraOptimization: value })
  }
}

const onCameraIdChange = (value) => {
  if (channelInfo.cameraTrack) {
    channelInfo.cameraTrack.setDevice(value).then(() => {
      const track = channelInfo.cameraTrack.getMediaStreamTrack();
      videoSettingRef.value!.srcObject = new MediaStream([track]);
      videoSettingRef.value!.play();
    });
  }
};

watchEffect(() => {
  if (!channelInfo.cameraTrack || !videoSettingRef.value) return;
  const track = channelInfo.cameraTrack.getMediaStreamTrack();
  videoSettingRef.value.srcObject = new MediaStream([track]);
  videoSettingRef.value.play();
});

const updateEncoder = () => {
  const isCameraPublish = channelInfo.publishedTracks.has(channelInfo.cameraTrack.getTrackId());
  const fn = () => {
    if (!channelInfo.cameraTrack) {
      message.info('请先创建摄像头轨道');
      return;
    }
    channelInfo.cameraTrack
      .setEncoderConfiguration({
        frameRate: formData.frameRate,
        dimension: formData.dimension,
        optimizationMode: formData.optimizationMode,
        maxBitrate: Number(formData.maxBitrate),
      })
      .then(() => {
        deviceInfo.$patch({ cameraMaxBitrate: channelInfo.cameraTrack.getMaxBitrate() });
        publish([channelInfo.cameraTrack as CameraVideoTrack]).then(() => {
          message.info('调整编码参数并重新发布成功');
          close();
        });
      });
  };
  if (isCameraPublish) {
    unpublish([channelInfo.cameraTrack as CameraVideoTrack]).then(() => {
      fn();
    });
  } else {
    fn();
  }
  deviceInfo.$patch({
    cameraFrameRate: formData.frameRate,
    cameraDimension: formData.dimension,
    cameraMaxBitrate: Number(formData.maxBitrate),
  });
};
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
