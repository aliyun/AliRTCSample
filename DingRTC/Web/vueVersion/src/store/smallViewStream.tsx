import { LocalVideoTrack, RemoteVideoTrack } from 'dingrtc';
import { defineStore } from 'pinia';

export const useSmallViewStreamStore = defineStore('smallView', {
  state: () => ({}) as { [userId: string]: RemoteVideoTrack },
  actions: {
    update(info: { [key: string]: RemoteVideoTrack | LocalVideoTrack }) {
      Object.keys(info).forEach((userId) => {
        this[userId] = info[userId];
      });
    },
  },
});
