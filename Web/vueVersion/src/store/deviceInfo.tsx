import type { DeviceInfo, VideoDimension } from 'dingrtc';
import { defineStore } from 'pinia';

interface deviceInfo {
  cameraId: string;
  micId: string;
  speakerId: string;
  cameraList: MediaDeviceInfo[];
  micList: MediaDeviceInfo[];
  speakerList: MediaDeviceInfo[];
  cameraFrameRate: number;
  cameraDimension: VideoDimension;
  screenFrameRate: number;
  screenDimension: VideoDimension;
  facingMode: string;
  cameraMaxBitrate?: number;
  screenMaxBitrate?: number;
  volume: number;
}

export const useDeviceInfoStore = defineStore('deviceInfo', {
  state: () =>
    ({
      cameraId: '',
      micId: '',
      speakerId: '',
      cameraList: [],
      speakerList: [],
      micList: [],
      cameraFrameRate: 15,
      cameraDimension: 'VD_640x480',
      screenFrameRate: 20,
      screenDimension: 'VD_1920x1080',
      facingMode: 'user',
      volume: 100,
    }) as deviceInfo,
  actions: {
    updateCameraList(info: DeviceInfo) {
      const index = this.cameraList.findIndex(
        (item: MediaDeviceInfo) => item.deviceId === info.device.deviceId,
      );
      if (info.state === 'active' && index === -1) {
        this.cameraList.push(info.device);
      } else if (info.state === 'inactive' && index !== -1) {
        this.cameraList.splice(index, 1);
      }
    },
    updateMicrophoneList(info: DeviceInfo) {
      const index = this.micList.findIndex(
        (item: MediaDeviceInfo) => item.deviceId === info.device.deviceId,
      );
      if (info.state === 'active' && index === -1) {
        this.micList.push(info.device);
      } else if (info.state === 'inactive' && index !== -1) {
        this.micList.splice(index, 1);
      }
    },
    updateSpeakerList(info: DeviceInfo) {
      const index = this.speakerList.findIndex(
        (item: MediaDeviceInfo) => item.deviceId === info.device.deviceId,
      );
      if (info.state === 'active' && index === -1) {
        this.speakerList.push(info.device);
      } else if (info.state === 'inactive' && index !== -1) {
        this.speakerList.splice(index, 1);
      }
    },
  },
});
