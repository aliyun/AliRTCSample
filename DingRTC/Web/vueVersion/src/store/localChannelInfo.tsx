import {
  CameraVideoTrack,
  LocalAudioTrack,
  LocalTrack,
  LocalVideoTrack,
  MicrophoneAudioTrack,
  RemoteAudioTrack,
} from 'dingrtc';
import { defineStore } from 'pinia';

interface LocalChannelInfoState {
  cameraTrack?: CameraVideoTrack;
  micTrack?: MicrophoneAudioTrack;
  screenTrack?: LocalVideoTrack;
  publishedTracks?: LocalTrack[];
  mcuAudioTrack?: RemoteAudioTrack;
  customVideoTrack?: LocalVideoTrack;
  customAudioTrack?: LocalAudioTrack;
  timeLeft?: number;
}

export const useLocalChannelInfoStore = defineStore('localChannelInfo', {
  state: () =>
    ({
      cameraTrack: undefined as CameraVideoTrack | undefined,
      screenTrack: undefined as LocalVideoTrack | undefined,
      customVideoTrack: undefined,
      customAudioTrack: undefined,
      micTrack: undefined,
      publishedTracks: [],
      mcuAudioTrack: undefined as RemoteAudioTrack | undefined,
      timeLeft: 0,
    }) as LocalChannelInfoState,
  actions: {
    updateStore(updateObject: Partial<LocalChannelInfoState>) {
      this.$patch(updateObject);
    },
  },
});
