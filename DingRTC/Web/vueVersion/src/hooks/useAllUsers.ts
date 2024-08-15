import { RemoteUser } from 'dingrtc';
import { ComputedRef, computed } from 'vue';

import { useCurrentUserInfo } from '@src/store/currentUserInfo';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { useRemoteChannelInfoStore } from '@src/store/remoteChannelnfo';

export const useAllUsers = () => {
  const localChannelStore = useLocalChannelInfoStore();
  const currentUserInfoStore = useCurrentUserInfo();
  const remoteChannelStore = useRemoteChannelInfoStore();
  const allUsers = computed(
    () =>
      [
        {
          userId: currentUserInfoStore.userId,
          videoTrack: localChannelStore.$state.cameraTrack,
          userName: currentUserInfoStore.userName,
          hasVideo: !!localChannelStore.cameraTrack,
          hasAudio: !!localChannelStore.micTrack,
          audioMuted: !localChannelStore.micTrack?.enabled,
          videoMuted: !localChannelStore.cameraTrack?.enabled,
          auxiliaryMuted: localChannelStore.screenTrack
            ? !localChannelStore.screenTrack?.enabled
            : false,
          hasAuxiliary: !!localChannelStore.screenTrack,
          auxiliaryTrack: localChannelStore.screenTrack,
        },
        ...remoteChannelStore.remoteUsers,
      ] as Partial<RemoteUser>[],
  );
  return allUsers;
};

export const useUserInfoByUserId = (userId: ComputedRef<string>) => {
  const allUsers = useAllUsers();
  const user = computed(() => {
    return allUsers.value.find((user) => user.userId === userId.value);
  });
  return user;
};
