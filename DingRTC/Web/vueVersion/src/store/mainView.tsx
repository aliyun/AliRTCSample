import { RemoteVideoTrack } from 'dingrtc';
import { defineStore } from 'pinia';

export enum DisplayType {
  Avatar = 'Avatar',
  Camera = 'Camera',
  Screen = 'Screen',
}

export const useMainViewStore = defineStore<
  'mainView',
  {
    userId: string;
    displayType: DisplayType;
    stream?: RemoteVideoTrack;
  }
>('mainView', {
  state: () => ({
    userId: '',
    displayType: DisplayType.Camera,
    stream: undefined,
  }),
});
