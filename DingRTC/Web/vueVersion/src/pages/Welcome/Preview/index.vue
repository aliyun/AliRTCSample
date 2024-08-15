<script setup lang="ts">
import { Avatar, Modal } from 'ant-design-vue';
import { onMounted, ref } from 'vue';

import { useDevice } from '@src/hooks/useDevices';
import { useCurrentUserInfo } from '@src/store/currentUserInfo';
import { isIOS, isWeixin } from '@src/utils/tools';

import Camera from './Camera.vue';
import MicPhone from './Micphone.vue';

const videoPlayerRef = ref<HTMLDivElement>();

const { openMicAndCameraSameTime, cameraEnabled } = useDevice('pre');

onMounted(() => {
  openMicAndCameraSameTime().then(([track]) => {
    if (isIOS() && isWeixin()) {
      Modal.info({
        title: '提示',
        content: 'iOS微信浏览器需要在回调内播放',
        okText: '确认',
        cancelText: '取消',
        onOk() {
          track.play(videoPlayerRef.value, { fit: 'cover' });
        },
        onCancel() {
          track.play(videoPlayerRef.value, { fit: 'cover' });
        },
      });
    } else {
      track?.play(videoPlayerRef.value, { fit: 'cover' });
    }
  });
});
const currentUserInfoStore = useCurrentUserInfo();
</script>

<template>
  <div class="previewCameraWrapper">
    <div ref="videoPlayerRef" :class="cameraEnabled ? 'camera' : 'avatar'">
      <Avatar :size="128">{{ currentUserInfoStore.userName }}</Avatar>
    </div>
    <div class="controls"><MicPhone /> <Camera /></div>
  </div>
</template>

<style lang="less">
.previewCameraWrapper {
  width: 700px;
  height: 500px;
  border-radius: 12px;
  position: relative;
  .avatar {
    height: 100%;
    width: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
    background-color: #dce1e6;
    border-radius: 12px;
  }
  @media (max-width: 768px) {
    flex: 0;
    width: auto;
  }
  .controls {
    position: absolute;
    bottom: 20px;
    left: 50%;
    transform: translateX(-50%);
    display: flex;
    gap: 30px;
  }
  .camera {
    border-radius: 12px;
    overflow: hidden;
    height: 500px;
    position: relative;
    .ant-avatar {
      display: none;
    }
  }
  @media (max-width: 768px) {
    /* 当屏幕宽度小于或等于 768px 时，改为上下布局 */
    .camera {
      max-height: 300px;
      width: 400px;
    }
  }

  .avatar {
    & video {
      display: none;
    }
  }
  .controls {
    height: 60px;
    display: flex;
    align-items: center;
    justify-content: space-around;
  }
}
</style>
