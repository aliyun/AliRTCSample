<template>
  <Row>
    <audio ref="audioRef" crossOrigin="anonymous" loop :src="testAudio" />
    <Form style="width: 100%" :label-col="{ span: 4 }" label-align="left" :model="formData">
      <Form.Item label="麦克风" name="micId">
        <Select
          v-model:value="formData.micId"
          :options="deviceInfo.micList.map((item) => ({ label: item.label, value: item.deviceId }))"
          @change="onMicIdChange"
        />
      </Form.Item>
      <Form.Item label="音频能量">
        <VolumeLevelBar :track="channelInfo.micTrack" />
      </Form.Item>
      <Divider />
      <Form.Item label="扬声器" class="testDevice">
        <Form.Item no-style name="speakerId">
          <Select
            v-model:value="formData.speakerId"
            :options="deviceInfo.speakerList.map((item) => ({ label: item.label, value: item.deviceId }))"
            @change="onSpearkerIdChange"
          />
        </Form.Item>
        <Button type="link" @click="testDevice">
          测试
        </Button>
      </Form.Item>
      <Form.Item label="音频能量">
        <VolumeLevelBar :track="testTrack" />
      </Form.Item>
      <Form.Item label="播放音量" name="playVolume">
        <Slider
          v-model:value="formData.playVolume"
          :max="1"
          :step="0.01"
          :min="0"
          @change="onPlayVolumeChange"
        />
      </Form.Item>
    </Form>
  </Row>
</template>
<script setup lang="ts">
import {
  Row,
  Divider,
  Form,
  Select,
  Slider,
  Button,
} from 'ant-design-vue';
import { useDeviceInfo, useChannelInfo } from '~/store';
import DingRTC, { LocalAudioTrack } from 'dingrtc';
import testAudio from '~/1638.mp3';
import VolumeLevelBar from './VolumeLevelBar.vue';
import { reactive, ref } from 'vue';


const deviceInfo = useDeviceInfo();
const channelInfo = useChannelInfo();
const audioRef = ref(null);
const testTrack = ref<LocalAudioTrack>(null);

const formData = reactive({
  micId: deviceInfo.micId,
  speakerId: deviceInfo.speakerId,
  playVolume: 1,
})

const testDevice = () => {
  audioRef.value.onplay = async () => {
    const captureStream: MediaStream = audioRef.value.captureStream();
    const track = captureStream.getAudioTracks()[0];
    const customTrack = await DingRTC.createCustomAudioTrack({ mediaStreamTrack: track });
    testTrack.value = customTrack;
    setTimeout(() => {
      audioRef.value.pause();
      testTrack.value = null;
    }, 4000);
  };
  if (audioRef.value.setSinkId && audioRef.value.sinkId !== deviceInfo.speakerId) {
    audioRef.value.setSinkId(deviceInfo.speakerId).then(() => {
      audioRef.value.play();
    });
  } else {
    audioRef.value.play();
  }
};
const onPlayVolumeChange = (value) => {
  channelInfo.mcuAudioTrack.setVolume(value);
}
const onSpearkerIdChange = (value) => {
  if (!channelInfo.mcuAudioTrack) return;
  channelInfo.mcuAudioTrack.setSpeaker(value);
  deviceInfo.$patch({ speakerId: value })
}
const onMicIdChange = (value) => {
  if (!channelInfo.micTrack) return;
  channelInfo.micTrack.setDevice(value);
  deviceInfo.$patch({ micId: value })
}
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>