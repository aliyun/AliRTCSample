<template>
  <Row :key="componentKey" class="videoSetting">
    <div class="videoWrapper" ref="refContainer">
    </div>
    <Form style="width: 100%" :label-col="{ span: 4 }" label-align="left" :model="cameraFormData">
      <Form.Item label="摄像头" name="cameraId">
        <Select
          v-model:value="cameraFormData.cameraId"
          :options="
            deviceInfo.cameraList.map((item) => ({ label: item.label, value: item.deviceId }))
          "
          @change="onCameraIdChange"
        />
      </Form.Item>
    </Form>
    <Divider orientation="left" plain>
      <Text>摄像头编码设置</Text>
    </Divider>
    <Form style="width: 100%" label-align="left" layout="horizontal" :model="cameraFormData">
      <Row :gutter="24">
        <Col :span="12">
          <Form.Item label="分辨率" name="dimension">
            <Select
              v-model:value="cameraFormData.dimension"
              :options="videoDimensions.map((item) => ({ label: item, value: item }))"
            />
          </Form.Item>
        </Col>
        <Col :span="8">
          <Form.Item label="帧率" name="frameRate">
            <Select
              v-model:value="cameraFormData.frameRate"
              :options="frameRates.map((item) => ({ label: item, value: item }))"
            />
          </Form.Item>
        </Col>
        <Col :span="4">
          <Button class="apply" @click="updateCameraEncoder"> 应用 </Button>
        </Col>
      </Row>
    </Form>
    <Divider orientation="left" plain>
      <Text>虚拟背景设置</Text>
    </Divider>
    <Form style="width: 100%" label-align="left" layout="horizontal">
      <Row justify="space-between">
        <Col>
          <Form.Item label="背景图片">
            <Upload
              :max-count="1"
              :file-list="imageFileList"
              accept=".png"
              :before-upload="beforeUpload"
              @change="(changeParam) => onImageFileChange(changeParam)"
            >
              <Button> <UploadOutlined />选择背景图片</Button>
            </Upload>
          </Form.Item>
        </Col>
        <Col>
          <Form.Item>
            <Button @click="onBkImageApply">应用虚拟背景</Button>
          </Form.Item>
        </Col>
      </Row>
    </Form>
    <Divider orientation="left" plain>
      <Text>共享编码设置</Text>
    </Divider>
    <Form style="width: 100%" label-align="left" layout="horizontal" :model="shareFormData">
      <Row :gutter="24">
        <Col :span="12">
          <Form.Item label="分辨率" name="dimension">
            <Select
              v-model:value="shareFormData.dimension"
              :options="videoDimensions.map((item) => ({ label: item, value: item }))"
            />
          </Form.Item>
        </Col>
        <Col :span="8">
          <Form.Item label="帧率" name="frameRate">
            <Select
              v-model:value="shareFormData.frameRate"
              :options="frameRates.map((item) => ({ label: item, value: item }))"
            />
          </Form.Item>
        </Col>
        <Col :span="4">
          <Button class="apply" @click="updateShareEncoder"> 应用 </Button>
        </Col>
      </Row>
    </Form>
  </Row>
</template>
<script lang="ts" setup>
import {
  Row,
  Typography,
  Divider, Form,
  Select, Button,
  Upload,
  UploadFile,
  UploadChangeParam,
  message,
  Col
} from 'ant-design-vue';
import { webUtils } from 'electron';
import { UploadOutlined } from '@ant-design/icons-vue';
import { onBeforeUnmount, onMounted, reactive, ref, toRaw } from 'vue';
import { videoDimensions, frameRates } from '~/constants';
import { useChannel } from '~/hooks/channel';
import { useDeviceInfo, useChannelInfo } from '~/store';

import DingRTC, { CameraVideoTrack, LocalVideoTrack, VirtualBackgroundMode } from 'dingrtc-electron-sdk';

const componentKey = ref(0);
const imageFileList = ref<UploadFile[]>([]);
const imageFilePath = ref('');

const { Text } = Typography;

const props = defineProps(['close']);
const { close } = props;
const refContainer = ref(null);
const deviceInfo = useDeviceInfo();

const cameraFormData = reactive({
  cameraId: deviceInfo.cameraId,
  dimension: deviceInfo.cameraDimension,
  frameRate: deviceInfo.cameraFrameRate,
});

const shareFormData = reactive({
  dimension: deviceInfo.screenDimension,
  frameRate: deviceInfo.screenFrameRate,
})

onMounted(() => {
  toRaw(channelInfo.cameraTrack)?.play(refContainer.value);
})

onBeforeUnmount(() => {
    // @ts-ignore
    toRaw(channelInfo.cameraTrack)?.stop(refContainer.value);
})

const channelInfo = useChannelInfo();
const { publish } = useChannel();
const onCameraIdChange = (value) => {
  if (channelInfo.cameraTrack) {
    toRaw(channelInfo.cameraTrack).setDevice(value)
  }
};

const beforeUpload = () => false;
const onImageFileChange = async ({ fileList }: UploadChangeParam<any>) => {
  if (!fileList.length) return;
  const filePath = webUtils.getPathForFile(fileList[0].originFileObj);
  imageFileList.value = fileList;
  imageFilePath.value = filePath;
};

const updateCameraEncoder = () => {
  const { frameRate, dimension } = cameraFormData;
  const isCameraPublish = channelInfo.publishedTracks.has(channelInfo.cameraTrack?.getTrackId());
  if (!channelInfo.cameraTrack) {
    message.info('请先创建摄像头轨道');
    return;
  }
  toRaw(channelInfo.cameraTrack)
    .setEncoderConfiguration({
      frameRate,
      dimension,
    })
    .then(() => {
      if (!isCameraPublish) {
        publish([channelInfo.cameraTrack as CameraVideoTrack]).then(() => {
          message.info('调整编码参数并重新发布成功');
          close();
        });
      }
    });
  deviceInfo.$patch({
    cameraFrameRate: frameRate,
    cameraDimension: dimension,
  });
};


const updateShareEncoder = () => {
  const { dimension, frameRate } = shareFormData;
  const isScreenPublish = channelInfo.publishedTracks.has(channelInfo.screenTrack.getTrackId());

  if (!channelInfo.screenTrack) {
    message.info('请先创建共享轨道');
    return;
  }
  toRaw(channelInfo.screenTrack)
    .setEncoderConfiguration({
      frameRate,
      dimension,
    })
    .then(() => {
      if (!isScreenPublish) {
        publish([channelInfo.screenTrack as LocalVideoTrack]).then(() => {
          message.info('调整编码参数并重新发布成功');
          close();
        });
      }
    });
  deviceInfo.$patch({
    screenDimension: dimension,
    screenFrameRate: frameRate,
  });
}

const onBkImageApply = () => {
  DingRTC.enableVirtualBackground(true, {
    mode: VirtualBackgroundMode.VirtualBackgroundModeReplace,
    imagePath: imageFilePath.value
  });
}

</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>
