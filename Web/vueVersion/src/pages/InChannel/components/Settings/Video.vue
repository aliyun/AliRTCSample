<template>
  <Row class="videoSetting">
    <div class="videoWrapper" ref="refContainer">
    </div>
    <Form style="width: 100%" label-align="left" :model="formData">
      <Form.Item label="摄像头" name="cameraId">
        <Select
          v-model:value="formData.cameraId"
          :options="
            deviceInfo.cameraList.map((item) => ({ label: item.label, value: item.deviceId }))
          "
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
      <Row justify="space-between">
        <Form.Item label="帧&nbsp&nbsp&nbsp&nbsp率" name="frameRate">
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
        <Form.Item label="镜像推流" name="mirror">
          <Checkbox v-model:checked="formData.mirror" />
        </Form.Item>
      </Row>
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
import {
  Row,
  Typography,
  Divider,
  Form,
  Select,
  InputNumber,
  Button,
  message,
  Checkbox,
} from 'ant-design-vue';
import { onBeforeUnmount, onMounted, reactive, ref } from 'vue';
import { useDeviceInfo, useChannelInfo } from '~/store';
import { videoDimensions, frameRates, optionzationModeList } from '~/constants';

const { Text } = Typography;

const deviceInfo = useDeviceInfo();
const refContainer = ref(null);

const formData = reactive({
  cameraId: deviceInfo.cameraId,
  maxBitrate: deviceInfo.cameraMaxBitrate,
  dimension: deviceInfo.cameraDimension,
  frameRate: deviceInfo.cameraFrameRate,
  mirror: deviceInfo.cameraMirror,
  optimizationMode: deviceInfo.cameraOptimization,
});
const channelInfo = useChannelInfo();

const onModeChange = (value) => {
  if (channelInfo.cameraTrack) {
    deviceInfo.$patch({ cameraOptimization: value });
  }
};

const onCameraIdChange = (value) => {
  channelInfo.cameraTrack?.setDevice(value)
};

onMounted(() => {
  channelInfo.cameraTrack?.play(refContainer.value);
})

onBeforeUnmount(() => {
    // @ts-ignore
  channelInfo.cameraTrack?.stop(refContainer.value);
})

const updateEncoder = () => {
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
      mirror: formData.mirror,
    })
    .then(() => {
      if (formData.maxBitrate) {
        deviceInfo.$patch({ cameraMaxBitrate: formData.maxBitrate });
      }
      message.info('调整编码参数成功');
    });

  deviceInfo.$patch({
    cameraFrameRate: formData.frameRate,
    cameraDimension: formData.dimension,
    cameraOptimization: formData.optimizationMode,
    cameraMaxBitrate: Number(formData.maxBitrate),
    cameraMirror: formData.mirror,
  });
};
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
