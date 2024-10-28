import { RemoteAudioTrack, RemoteUser } from 'dingrtc';
import { defineStore } from 'pinia';

interface IRemoteChannelInfo {
  mcuAudioTrack: RemoteAudioTrack | null;
  remoteUsers: RemoteUser[];
  speakers?: string[];
}

export const useRemoteChannelInfoStore = defineStore('remoteChannelInfo', {
  state: () =>
    ({
      mcuAudioTrack: null,
      remoteUsers: [],
      speakers: [],
    }) as IRemoteChannelInfo,
});
