import { LocalVideoTrack } from 'dingrtc';
import { watchEffect } from 'vue';

import { useClient } from '@src/store/client';
import { useLocalChannelInfoStore } from '@src/store/localChannelInfo';

export const useScreenTrackListener = () => {
  const localChannelInfo = useLocalChannelInfoStore();
  const screenTrack = localChannelInfo.screenTrack;
  const client = useClient();

  watchEffect(() => {
    if (!screenTrack) {
      return;
    }
    const clearTrack = () => {
      screenTrack.stop();
      client.unpublish(screenTrack as LocalVideoTrack);
      localChannelInfo.$patch({
        screenTrack: undefined,
      });
    };
    screenTrack.on('track-ended', clearTrack);
    return () => {
      screenTrack.off('track-ended', clearTrack);
    };
  });
};
