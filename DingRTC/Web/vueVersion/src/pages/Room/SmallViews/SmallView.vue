<script setup lang="ts">
import { Avatar } from 'ant-design-vue';
import { RemoteUser } from 'dingrtc';
import { ref, computed, toRef, watch } from 'vue';

import {
  VideoCameraOutlined,
  AudioOutlined,
  FundProjectionScreenOutlined,
} from '@ant-design/icons-vue';
import { useUserInfoByUserId } from '@src/hooks/useAllUsers';
import { useDevice } from '@src/hooks/useDevices';
import { useMainUser } from '@src/hooks/useMainUser';
import { useDbClickSmallView } from '@src/hooks/useMainView';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { DisplayType, useMainViewStore } from '@src/store/mainView';
import { useSmallViewStreamStore } from '@src/store/smallViewStream';

import { playTrack } from './playTrack';

// import OperatePanel from "./OperatorPanel.vue";

const props = defineProps<{
  isLocal: boolean;
  user: Partial<RemoteUser>;
  micEnabled: boolean;
  videoEnabled: boolean;
  shareEnabled: boolean;
  userId: string;
}>();

const currentUserId = computed(() => props.userId);
const currentUserInfo = useUserInfoByUserId(currentUserId);

const localInfoStore = useLocalChannelInfoStore();
const { cameraEnabled: localCameraEnable, micEnabled: localMicEnable } = useDevice();
const cameraIconEnable = computed(() => {
  if (props.isLocal) return localCameraEnable.value;
  else {
    return props.videoEnabled;
  }
});

const micIconEnable = computed(() => {
  if (props.isLocal) return localMicEnable.value;
  else {
    return props.micEnabled;
  }
});

const auxiliaryEnable = computed(() => {
  if (props.isLocal) return !!localInfoStore.screenTrack?.enabled;
  else {
    return props.shareEnabled;
  }
});

const videoRef = ref<HTMLDivElement>();

const mainViewStore = useMainViewStore();
const mainUser = useMainUser();
const shouldShowAvatar = computed(() => {
  if (props.userId === mainUser.value.userId) {
    if (
      auxiliaryEnable.value &&
      !cameraIconEnable.value &&
      mainViewStore.displayType === DisplayType.Screen
    ) {
      return true;
    } else if (
      cameraIconEnable.value &&
      !auxiliaryEnable.value &&
      mainViewStore.displayType === DisplayType.Camera
    ) {
      return true;
    } else if (!cameraIconEnable.value && !auxiliaryEnable.value) {
      return true;
    } else {
      return false;
    }
  } else {
    if (props.shareEnabled || props.videoEnabled) {
      return false;
    }
    return true;
  }
});

const mainViewUser = useMainUser();
const isMainUser = computed(() => currentUserInfo.value.userId === mainViewUser.value.userId);
const smallViewStore = useSmallViewStreamStore();

watch(
  [isMainUser, () => mainViewStore.displayType, cameraIconEnable, auxiliaryEnable],
  () => {
    const userVideoTrack = currentUserInfo.value.videoTrack;
    const userAuxiliaryTrack = currentUserInfo.value.auxiliaryTrack;
    if (isMainUser.value) {
      if (mainViewStore.displayType === DisplayType.Avatar) {
        if (cameraIconEnable.value) {
          smallViewStore.update({
            [props.userId]: userVideoTrack,
          });
          playTrack({ track: userVideoTrack, wrapper: videoRef });
        }
      } else if (mainViewStore.displayType === DisplayType.Camera && !!userAuxiliaryTrack) {
        smallViewStore.update({
          [props.userId]: userAuxiliaryTrack,
        });
        playTrack({
          track: userAuxiliaryTrack,
          wrapper: videoRef,
        });
      } else if (mainViewStore.displayType === DisplayType.Screen && !!userAuxiliaryTrack) {
        smallViewStore.update({
          [props.userId]: userVideoTrack,
        });
        playTrack({
          track: userVideoTrack,
          wrapper: videoRef,
        });
      }
    }
  },
  {
    immediate: true,
  },
);

watch(
  [isMainUser, cameraIconEnable, auxiliaryEnable],
  () => {
    const userVideoTrack = currentUserInfo.value.videoTrack;
    const userAuxiliaryTrack = currentUserInfo.value.auxiliaryTrack;
    if (!isMainUser.value) {
      if (currentUserInfo.value.videoTrack) {
        smallViewStore.update({
          [props.userId]: userVideoTrack,
        });
        playTrack({
          track: userVideoTrack,
          wrapper: videoRef,
        });
      } else if (currentUserInfo.value.auxiliaryTrack) {
        smallViewStore.update({
          [props.userId]: userAuxiliaryTrack,
        });
        playTrack({
          track: userAuxiliaryTrack,
          wrapper: videoRef,
        });
      }
    }
  },
  {
    immediate: true,
  },
);

const updateMainView = useDbClickSmallView({
  userId: props.userId,
  cameraEnableRef: cameraIconEnable,
  auxiliaryEnableRef: auxiliaryEnable,
  user: toRef(props, 'user'),
});
</script>

<template>
  <div class="smallViewInnerWrapper" @dblclick="() => updateMainView()">
    <div class="userName">
      {{ props.user?.userName + ' : ' + props.userId }}
    </div>
    <div class="videoWrapper" ref="videoRef"></div>
    <div v-if="shouldShowAvatar" class="avatarWrapper">
      <Avatar :size="40">{{ currentUserInfo?.userName }}</Avatar>
    </div>

    <div class="smallIconsWrapper">
      <AudioOutlined :style="{ color: micIconEnable ? 'red' : 'black' }" />
      <VideoCameraOutlined
        :style="{
          color: cameraIconEnable ? 'red' : 'black',
        }"
      />
      <FundProjectionScreenOutlined
        :style="{
          color: auxiliaryEnable ? 'red' : 'black',
        }"
      />
    </div>
  </div>
</template>

<style scoped>
.smallViewInnerWrapper {
  width: 184.889px;
  height: 104px;
  padding-top: 16px;
  padding-bottom: 12px;
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  align-items: center;
  background-color: #dce1e6;
  position: relative;
  border-radius: 12px;
  overflow: hidden;
}
.videoWrapper {
  position: absolute;
  left: 0;
  top: 0;
  bottom: 0%;
  right: 0;
  margin: auto;
  width: 100%;
  height: 100%;
}
.avatarWrapper {
  background-color: #dce1e6;
  position: absolute;
  left: 0;
  top: 0;
  bottom: 0%;
  right: 0;
  margin: auto;
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 1;
}
.userName {
  position: absolute;
  top: 4px;
  left: 4px;
  font-size: 12px;
  color: black;
  z-index: 10;
  background-color: rgba(255, 255, 255, 0.4);
  padding: 4px;
  border-radius: 4px;
}

.smallIconsWrapper {
  position: absolute;
  bottom: 4px;
  left: 4px;
  padding: 0 4px;
  border-radius: 4px;
  background-color: rgba(255, 255, 255, 0.2);
  z-index: 1;
  display: flex;
  gap: 20px;
  height: 24px;
  justify-content: left;
  align-items: center;
}
</style>
