<script setup lang="ts">
import { useAllUsers } from '@src/hooks/useAllUsers';
import { useCurrentUserInfo } from '@src/store/currentUserInfo';

import SmallView from './SmallView.vue';

const currentUserInfoStore = useCurrentUserInfo();

const allUsers = useAllUsers();
</script>

<template>
  <div class="smallViewWrapper">
    <SmallView
      v-for="user in allUsers"
      :key="user.userId"
      :userId="user.userId"
      :user="user"
      :isLocal="user.userId === currentUserInfoStore.userId"
      :micEnabled="!!user.hasAudio && !user.audioMuted"
      :videoEnabled="!!user.hasVideo && !user.videoMuted && !!user.videoTrack"
      :shareEnabled="!!user.hasAuxiliary && !user.auxiliaryMuted && !!user.auxiliaryTrack"
    />
  </div>
</template>

<style scoped>
.smallViewWrapper {
  position: relative;
  width: 100%;
  padding-top: 16px;
  padding-bottom: 12px;
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  align-items: center;
  background-color: #f5f7fa;
}
</style>
