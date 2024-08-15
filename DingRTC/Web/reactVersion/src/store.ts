import DingRTC, { CameraVideoTrack, DingRTCClient, LocalAudioTrack, LocalTrack, LocalVideoTrack, MicrophoneAudioTrack, RemoteAudioTrack, RemoteUser, VideoDimension, NetworkQuality } from "dingrtc";
import { atom } from "recoil";
import { isIOS, isMobile, isWeixin, logLevel, parseSearch } from "./utils/tools";
import configJson from '~/config.json'

DingRTC.setLogLevel(logLevel);

export interface ILocalChannelInfo {
  cameraTrack?: CameraVideoTrack;
  micTrack?: MicrophoneAudioTrack;
  screenTrack?: LocalVideoTrack;
  customVideoTrack?: LocalVideoTrack;
  customAudioTrack?: LocalAudioTrack;
  publishedTracks?: LocalTrack[];
  timeLeft?: number;
  networkQuality: NetworkQuality,
  rtcStats: RTCStats;
}

interface Resolution {
  width: number;
  height: number; 
}

export interface RTCStats {
  localCameraFPS?: number;
  localCameraResolution?: Resolution;
  localCameraBitrate?: number;
  localScreenFPS?: number;
  localScreenResolution?: Resolution;
  localScreenBitrate?: number;
  localBitrate?: number;
  remoteBitrate?: number;
  remoteCameraFPS?: number;
  remoteCameraResolution?: Resolution;
  remoteCameraBitrate?: number;
  remoteScreenFPS?: number;
  remoteScreenResolution?: Resolution;
  localAudioBitrate?: number;
  localAudioLevel?: number;
  remoteCamerateBitrate?: number;
  remoteScreenBitrate?: number;
  remoteAudioBitrate?: number;
  remoteAudioLevel?: number;
  loss?: number;
  rtt?: number;
}

interface IDeviceInfo {
  cameraId: string;
  speakerId: string;
  micId: string;
  screenFrameRate: number;
  screenDimension: VideoDimension;
  cameraFrameRate: number;
  cameraDimension: VideoDimension;
  screenMaxBitrate: number;
  cameraMaxBitrate: number,
  cameraList: MediaDeviceInfo[];
  speakerList: MediaDeviceInfo[];
  micList: MediaDeviceInfo[];
  facingMode: 'user' | 'environment';
}

interface IRemoteChannelInfo {
  mcuAudioTrack: RemoteAudioTrack;
  remoteUsers: RemoteUser[];
  speakers?: string[];
  subscribeAllVideo?: boolean;
}

interface IGlobalFlag {
  joined: boolean;
  hideToolBar: boolean;
}

export const client = atom<DingRTCClient>({
  key: 'Icient',
  dangerouslyAllowMutability: true,
  default: DingRTC.createClient(),
})

export const constantConfig = atom({
  key: 'IconstantConfig',
  default: {
    isMobile: isMobile(),
    hideLog: logLevel === 'none',
    isIOS: isIOS(),
    isWeixin: isWeixin(),
  }
})

export const currentUserInfo = atom({
  key: 'IcurrentUserInfo',
  default: {
    appId: parseSearch('appId') || configJson.appId || '',
    userId: parseSearch('userId') || configJson.userId || `${Math.ceil(Math.random() * 10000)}`,
    userName: parseSearch('userName') || configJson.userName || `Web-${Math.ceil(Math.random() * 100)}`,
    channel: parseSearch('channelId') || configJson.channelId || `${Math.ceil(Math.random() * 10000)}`,
  },
  dangerouslyAllowMutability: true,
});

export const deviceInfo = atom<IDeviceInfo>({
  key: 'IdeviceInfo',
  dangerouslyAllowMutability: true,
  default: {
    cameraId: '',
    micId: '',
    speakerId: '',
    cameraList: [],
    speakerList: [],
    micList: [],
    cameraFrameRate: 15,
    cameraMaxBitrate: 0,
    cameraDimension: 'VD_640x480',
    screenFrameRate: 5,
    screenMaxBitrate: 0,
    screenDimension: 'VD_1920x1080',
    facingMode: 'user',
  }
})

export const localChannelInfo = atom<ILocalChannelInfo>({
  key: 'ILocalChannelInfo',
  dangerouslyAllowMutability: true,
  default: {
    cameraTrack: null,
    screenTrack: null,
    customVideoTrack: null,
    customAudioTrack: null,
    micTrack: null,
    timeLeft: 0,
    networkQuality: 1,
    publishedTracks: [],
    rtcStats: {},
  }
});

export const remoteChannelInfo = atom<IRemoteChannelInfo>({
  key: 'IremoteChannelInfo',
  dangerouslyAllowMutability: true,
  default: {
    mcuAudioTrack: null,
    remoteUsers: [],
    subscribeAllVideo: true,
    speakers: [],
  }
})

export const globalFlag = atom<IGlobalFlag>({
  key: 'IglobalFlag',
  default: {
    joined: false,
    hideToolBar: false,
  }
});

export interface MainViewPrefer {
  userId: string;
  prefer: 'camera' | 'auxiliary',
}

export const mainViewPrefer = atom<MainViewPrefer>({
  key: 'IMainViewPrefer',
  dangerouslyAllowMutability: true,
  default: {
    userId: '',
    prefer: 'auxiliary',
  },
})
