<script lang="ts" setup>
import { Button } from 'ant-design-vue';
import { computed, onMounted, ref } from 'vue';

import {
  AudioOutlined,
  VideoCameraOutlined,
  FundProjectionScreenOutlined,
} from '@ant-design/icons-vue';
import { isInRoom } from '@src/Router';
import DeviceSelector from '@src/components/DeviceSelector.vue';
import GapLine from '@src/components/GapLine.vue';
import RichBtn from '@src/components/RichBtn.vue';
import { useDevice } from '@src/hooks/useDevices';
import { useLocalChannel } from '@src/hooks/useLocalChannel';
import { useClient } from '@src/store/client';
import { useDeviceInfoStore } from '@src/store/deviceInfo';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { useRemoteChannelInfoStore } from '@src/store/remoteChannelnfo';

const localInfoStore = useLocalChannelInfoStore();
const { cameraTrack, micTrack, screenTrack } = useLocalChannel();

const { initDeviceList, operateMic, operateCamera, operateScreen } = useDevice('in');
const deviceInfoStore = useDeviceInfoStore();
const localChannelStore = useLocalChannelInfoStore();
const remoteChannelStore = useRemoteChannelInfoStore();
const IClient = useClient();
const micList = computed(() => deviceInfoStore.micList);
const cameraList = computed(() => deviceInfoStore.cameraList);
const speakList = computed(() => deviceInfoStore.speakerList);

const micOpen = computed(() => localInfoStore.micTrack?.enabled);
const cameraOpen = computed(() => localInfoStore.cameraTrack?.enabled);
const screenOpen = computed(() => localInfoStore.screenTrack?.enabled);

onMounted(() => {
  initDeviceList({ mic: true, camera: true });
});

const updateMicDevice = (micId: string) => {
  deviceInfoStore.$patch({
    micId,
  });
  localChannelStore.micTrack?.setDevice(micId);
};

const updateCameraDevice = (cameraId: string) => {
  deviceInfoStore.$patch({
    cameraId,
  });
  localChannelStore.cameraTrack?.setDevice(cameraId);
};

const updateSpeakDevice = (speakerId: string) => {
  deviceInfoStore.$patch({
    speakerId,
  });
  remoteChannelStore.mcuAudioTrack?.setSpeaker(speakerId);
};

const leaveLoading = ref<boolean>(false);

const onLeave = async () => {
  leaveLoading.value = true;
  await IClient.leave();
  cameraTrack?.close();
  micTrack?.close();
  screenTrack?.close();
  remoteChannelStore.mcuAudioTrack?.stop();
  remoteChannelStore.remoteUsers.forEach((user) => {
    if (user.auxiliaryTrack) {
      user.auxiliaryTrack?.stop();
    }
    if (user.videoTrack) {
      user.videoTrack?.stop();
    }
  });
  localChannelStore.$reset();
  remoteChannelStore.$reset();
  leaveLoading.value = false;
  isInRoom.value = false;
};
</script>

<template>
  <div class="bottomWrapper">
    <div class="btnWrapper" @click="operateMic()">
      <RichBtn :hasSelector="true" :text="micOpen ? '关闭麦克风' : '开启麦克风'">
        <template v-slot:BtnIcon>
          <AudioOutlined
            :style="{
              color: micOpen ? 'red' : 'black',
              fontSize: '22px',
            }"
          />
        </template>
        <template #panelName>
          <DeviceSelector
            :list="micList"
            title="请选择麦克风"
            type="micId"
            :updateDevice="updateMicDevice"
          ></DeviceSelector>
          <GapLine></GapLine>
          <DeviceSelector
            :list="speakList"
            title="请选择扬声器"
            type="speakerId"
            :updateDevice="updateSpeakDevice"
          ></DeviceSelector>
        </template>
      </RichBtn>
    </div>
    <div class="btnWrapper" @click="operateCamera()">
      <RichBtn :hasSelector="true" :text="cameraOpen ? '关闭摄像头' : '开启摄像头'">
        <template v-slot:BtnIcon>
          <VideoCameraOutlined
            :style="{
              color: cameraOpen ? 'red' : 'black',
              fontSize: '22px',
            }"
          />
        </template>
        <template #panelName>
          <DeviceSelector
            :list="cameraList"
            title="请选择摄像头"
            type="cameraId"
            :updateDevice="updateCameraDevice"
          ></DeviceSelector>
        </template>
      </RichBtn>
    </div>
    <div class="btnWrapper" @click="operateScreen()">
      <RichBtn :hasSelector="false" :text="screenOpen ? '关闭共享' : '屏幕共享'">
        <template v-slot:BtnIcon>
          <FundProjectionScreenOutlined
            :style="{
              color: screenOpen ? 'red' : 'black',
              fontSize: '20px',
            }"
          />
        </template>
      </RichBtn>
    </div>
    <Button danger type="primary" :loading="leaveLoading" class="leaveBtn" @click="onLeave"
      >离开</Button
    >
    <Setting />
  </div>
</template>

<style scoped lang="less">
.bottomWrapper {
  height: 60px;
  width: 100vw;
  display: flex;
  background: #f5f7fa;
  justify-content: center;
  align-items: center;
  position: absolute;
  bottom: 0;
  z-index: 10;
  .btnWrapper {
    margin: 0 5px;
  }
}

.publishOperator {
  height: 36px;
  display: flex;
  align-items: center;
  padding-left: 8px;
  &:hover {
    cursor: pointer;
    background-color: #b3b3b4;
    border-radius: 4px;
  }
}
.leaveBtn {
  position: absolute;
  right: 40px;
}
</style>
