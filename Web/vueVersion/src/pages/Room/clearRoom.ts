import { isInRoom } from '@src/Router';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';
import { useMainViewStore } from '@src/store/mainView';
import { useRemoteChannelInfoStore } from '@src/store/remoteChannelnfo';

export const clearRoom = () => {
  const localChannelInfoStore = useLocalChannelInfoStore();
  const remoteChannelInfoStore = useRemoteChannelInfoStore();
  const mainViewStore = useMainViewStore();

  if (localChannelInfoStore.cameraTrack) {
    localChannelInfoStore.cameraTrack.close();
  }
  if (localChannelInfoStore.micTrack) {
    localChannelInfoStore.micTrack.close();
  }
  if (localChannelInfoStore.screenTrack) {
    localChannelInfoStore.screenTrack.close();
  }
  if (localChannelInfoStore.mcuAudioTrack) {
    localChannelInfoStore.mcuAudioTrack.stop();
  }

  remoteChannelInfoStore.remoteUsers?.forEach((user) => {
    if (user.auxiliaryTrack) {
      user.auxiliaryTrack.stop();
    }
    if (user.videoTrack) {
      user.videoTrack.stop();
    }
  });
  localChannelInfoStore.$reset();
  mainViewStore.$reset();
  remoteChannelInfoStore.$reset();
  isInRoom.value = false;
};
