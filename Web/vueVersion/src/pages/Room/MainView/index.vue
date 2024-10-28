<script setup lang="ts">
import { Avatar } from 'ant-design-vue';
import { ref, computed, watch } from 'vue';

import { useUserInfoByUserId } from '@src/hooks/useAllUsers';
import { useMainViewStore, DisplayType } from '@src/store/mainView';

import { playTrack } from '../SmallViews/playTrack';

const mainViewStore = useMainViewStore();
const wrapperRef = ref<HTMLDivElement>();
const mainViewUserId = computed(() => mainViewStore.userId);
const mainUser = useUserInfoByUserId(mainViewUserId);

watch(
  [() => mainViewStore.userId, () => mainViewStore.displayType, mainUser, wrapperRef],
  () => {
    const mainVideoTrack = mainUser.value.videoTrack;
    const mainAuxiliaryTrack = mainUser.value.auxiliaryTrack;
    const mainUserId = mainViewStore.userId;
    if (mainUserId && wrapperRef.value) {
      if (mainViewStore.displayType === DisplayType.Camera) {
        mainViewStore.$patch({ stream: mainVideoTrack });
        playTrack({ track: mainVideoTrack, wrapper: wrapperRef, objectFit: 'contain' });
      } else if (mainViewStore.displayType === DisplayType.Screen) {
        mainViewStore.$patch({ stream: mainAuxiliaryTrack });
        playTrack({ track: mainAuxiliaryTrack, wrapper: wrapperRef, objectFit: 'contain' });
      }
    }
  },
  {
    immediate: true,
  },
);

const shouldShowAvatar = computed(() => {
  const shareEnabled =
    mainUser.value.hasAuxiliary &&
    !mainUser.value.auxiliaryMuted &&
    !!mainUser.value.auxiliaryTrack;

  const videoEnabled =
    mainUser.value.hasVideo && !mainUser.value.videoMuted && !!mainUser.value.videoTrack;
  return (
    mainViewStore.displayType === DisplayType.Avatar ||
    (!videoEnabled && mainViewStore.displayType === DisplayType.Camera) ||
    (!shareEnabled && mainViewStore.displayType === DisplayType.Screen)
  );
});
</script>

<template>
  <div style="position: absolute; top: 20px; color: red; z-index: 30">
    {{ mainViewStore.userId }}
  </div>
  <div ref="wrapperRef" class="videoWrapper"></div>
  <div class="mainViewAvatar" v-if="shouldShowAvatar">
    <Avatar :size="150">{{ mainUser?.userName }}</Avatar>
  </div>
</template>
<style scoped>
.mainViewAvatar {
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
  position: absolute;
  z-index: 1;
  background-color: #dce1e6;
}
.videoWrapper {
  width: 100%;
  height: 100%;
  object-fit: cover;
  position: absolute;
}
</style>
