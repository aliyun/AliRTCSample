<script setup lang="ts">
import { Form, Select, InputNumber } from 'ant-design-vue';
import { VideoDimension } from 'dingrtc';
import { reactive, ref } from 'vue';

import { videoDimensions } from '@src/constant';
import { useDeviceInfoStore } from '@src/store';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';

interface IFormState {
  dimension: VideoDimension;
  frameRate: number;
}

const props = defineProps<{
  dimension: VideoDimension;
  frameRate: number;
  isPublished: boolean;
  publish: () => void;
  unPublish: () => void;
  type: 'camera' | 'screen';
}>();

const dimensionOptions = videoDimensions.map((item) => ({
  label: item,
  value: item,
}));

const localChannelStore = useLocalChannelInfoStore();
const deviceInfo = useDeviceInfoStore();

const formState = reactive<IFormState>({
  dimension: props.dimension,
  frameRate: props.frameRate,
});

const formRef = ref();

const updateEncode = (value: IFormState) => {
  if (props.isPublished) {
    // FIXME: 完善toast
    // Toast.info("推流过程中不能调整编码");
    return;
  }
  localChannelStore[
    (props.type + 'Track') as 'cameraTrack' | 'screenTrack'
  ]?.setEncoderConfiguration({
    frameRate: value.frameRate,
    dimension: value.dimension,
  });
  deviceInfo.$patch({
    [(props.type + 'Dimension') as 'cameraDimension' | 'screenDimension']: value.dimension,
    [(props.type + 'FrameRate') as 'cameraFrameRate' | 'screenFrameRate']: value.frameRate,
  });
  props.publish();
};

const resetValue = () => {
  formRef.value.resetFields();
};
</script>

<template>
  <div class="selectorTitle">调整视频编码</div>
  <div v-if="!props.isPublished">
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
    <Button type="primary" @click="props.unPublish()">{{ '取消发布' }}</Button>
  </div>
</template>

<style scoped>
.selectorTitle {
  background-color: azure;
}
</style>
