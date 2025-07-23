<template>
  <Row>
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
          {{ !playTesting ? '测试' : '停止' }} 
        </Button>
      </Form.Item>
      <Form.Item label="音频能量">
        <VolumeLevelBar :track="mixedPlayoutTrack" :level="playTestingLevel" />
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
import { useDeviceInfo, useChannelInfo, useGlobalFlag } from '~/store';
import VolumeLevelBar from './VolumeLevelBar.vue';
import { reactive, ref, toRaw, onMounted, onUnmounted } from 'vue';
//import testWav from '/playout_test.wav?asset';

const deviceInfo = useDeviceInfo();
const channelInfo = useChannelInfo();
const globalFlag = useGlobalFlag();
const playTesting = ref(false);
const playVolumeTimer = ref(null);
const playTestingLevel = ref(0);

const mixedPlayoutTrack = {
  getVolumeLevel: () => {
    if (playTesting.value) {
      return channelInfo.speakerTrack?.getVolumeLevel();
    } else {
      return channelInfo.mcuAudioTrack?.getVolumeLevel();
    }
  }
}

onMounted(() => {
  playVolumeTimer.value = setInterval(() => {
    formData.playVolume = toRaw(channelInfo.speakerTrack).getVolume();
  }, 500);
})

onUnmounted(() => {
  clearInterval(playVolumeTimer.value)
  if (playTesting.value) {
    toRaw(channelInfo.speakerTrack).stopTest();
    playTesting.value = false;
  }
})

const formData = reactive({
  micId: deviceInfo.micId,
  speakerId: deviceInfo.speakerId,
  playVolume: toRaw(channelInfo.speakerTrack).getVolume(),
})

const testDevice = () => {
  if (playTesting.value) {
    toRaw(channelInfo.speakerTrack).stopTest();
    playTestingLevel.value = 0;
  } else {
    let testWav = process.resourcesPath + '/extraResources/playout_test.wav';
    if (globalFlag.dev) {
      testWav = process.cwd() + '/src/renderer/public/playout_test.wav';
      console.info('+++++ playout source', testWav);
    }
    toRaw(channelInfo.speakerTrack).startTest(testWav);
  }
  playTesting.value = !playTesting.value;
};
const onPlayVolumeChange = (value) => {
  toRaw(channelInfo.speakerTrack).setVolume(value);
}
const onSpearkerIdChange = (value) => {
  if (!channelInfo.mcuAudioTrack) return;
  toRaw(channelInfo.speakerTrack).setDevice(value);
  deviceInfo.$patch({ speakerId: value })
}
const onMicIdChange = (value) => {
  if (!channelInfo.micTrack) return;
  toRaw(channelInfo.micTrack)?.setDevice(value);
  deviceInfo.$patch({ micId: value })
}
</script>
<style lang="less" scoped>
@import url('./index.module.less');
</style>