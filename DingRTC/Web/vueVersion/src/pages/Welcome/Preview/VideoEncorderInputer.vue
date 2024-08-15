<script setup lang="ts">
import { Form, Select, InputNumber, message as Toast } from 'ant-design-vue';
import { LocalVideoTrack } from 'dingrtc';
import { reactive, ref } from 'vue';

import { videoDimensions } from '@src/constant';
import { useLocalChannel } from '@src/hooks/useLocalChannel';
import { useDeviceInfoStore } from '@src/store';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';

interface IFormState {
  dimension: (typeof videoDimensions)[number];
  frameRate: number;
}

const dimensionOptions = videoDimensions.map((item) => ({
  label: item,
  value: item,
}));

const deviceInfoStore = useDeviceInfoStore();

const localChannelStore = useLocalChannelInfoStore();
const deviceInfo = useDeviceInfoStore();
const formState = reactive<IFormState>({
  dimension: deviceInfoStore.cameraDimension,
  frameRate: deviceInfoStore.cameraFrameRate,
});

const formRef = ref();

const { publishedTracks, unPublish, cameraTrack } = useLocalChannel();
const isCameraPublish = publishedTracks?.includes(localChannelStore.cameraTrack as LocalVideoTrack);

const updateEncode = (value: IFormState) => {
  if (isCameraPublish) {
    Toast.info('推流过程中不能调整编码');
    return;
  }
  localChannelStore.cameraTrack?.setEncoderConfiguration({
    frameRate: value.frameRate,
    dimension: value.dimension,
  });
  deviceInfo.$patch({
    cameraDimension: value.dimension,
    cameraFrameRate: value.frameRate,
  });
};

const unPublishCamera = () => {
  unPublish([cameraTrack as LocalVideoTrack]);
};

const resetValue = () => {
  formRef.value.resetFields();
};
</script>

<template>
  <div class="selectorTitle">调整视频编码</div>
  <div v-if="isCameraPublish">
    <Form
      ref="formRef"
      :model="formState"
      :label-col="{ span: 8 }"
      :wrapper-col="{ span: 16 }"
      autocomplete="off"
    >
      <Form.Item required name="frameRate" label="帧率">
        <InputNumber v-model:value="formState.frameRate" />
      </Form.Item>
      <Form.Item required name="dimension" label="分辨率">
        <Select :options="dimensionOptions" v-model:value="formState.dimension"></Select>
      </Form.Item>
      <Button type="primary" @click="updateEncode(formState)">{{ '发布' }}</Button>
      <Button @click="resetValue()">重置</Button>
    </Form>
  </div>
  <div v-else>
    <Button type="primary" @click="unPublishCamera()">{{ '取消发布' }}</Button>
  </div>
</template>
<style scoped>
.selectorTitle {
  background-color: azure;
}
</style>
