import DingRTC, { CameraVideoTrack, DingRTCClient, LocalTrack, LocalVideoTrack, MicrophoneAudioTrack, RemoteAudioTrack, RemoteUser, RemoteVideoTrack, VideoDimension } from "dingrtc";
import { atom } from "recoil";
import { isIOS, isMobile, isWeixin, logLevel, parseSearch } from "./utils/tools";

export const dingAppConf = {
  appId: parseSearch('appId') || '******',
  env: parseSearch('env') || 'onertcOnline',
};


DingRTC.setLogLevel(logLevel);

interface ILocalChannelInfo {
  cameraTrack?: CameraVideoTrack;
  micTrack?: MicrophoneAudioTrack;
  screenTrack?: LocalVideoTrack;
  publishedTracks?: LocalTrack[];
}

interface IDeviceInfo {
  cameraId: string;
  speakerId: string;
  micId: string;
  screenFrameRate: number;
  screenDimension: VideoDimension;
  cameraFrameRate: number;
  cameraDimension: VideoDimension;
  cameraList: MediaDeviceInfo[];
  speakerList: MediaDeviceInfo[];
  micList: MediaDeviceInfo[];
  facingMode: 'user' | 'environment';
}

interface IRemoteChannelInfo {
  mcuAudioTrack: RemoteAudioTrack;
  remoteUsers: RemoteUser[];
  speakers?: string[];
}

interface IGlobalFlag {
  joined: boolean;
  showLog: boolean;
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
    env: dingAppConf.env,
    isIOS: isIOS(),
    isWeixin: isWeixin(),
  }
})

export const currentUserInfo = atom({
  key: 'IcurrentUserInfo',
  default: {
    appId: dingAppConf.appId,
    userId: `${Math.ceil(Math.random() * 10000)}`,
    userName: `Web-${Math.ceil(Math.random() * 100)}`,
    channel: `${Math.ceil(Math.random() * 10000)}`,
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
    cameraDimension: 'VD_640x480',
    screenFrameRate: 5,
    screenDimension: 'VD_1920x1080',
    facingMode: 'user',
  }
})

export const localChannelInfo = atom<ILocalChannelInfo>({
  key: 'IlocalChanel',
  dangerouslyAllowMutability: true,
  default: {
    cameraTrack: null,
    screenTrack: null,
    micTrack: null,
    publishedTracks: [],
  }
});

export const remoteChannelInfo = atom<IRemoteChannelInfo>({
  key: 'IremoteChannelInfo',
  dangerouslyAllowMutability: true,
  default: {
    mcuAudioTrack: null,
    remoteUsers: [],
    speakers: [],
  }
})

export const globalFlag = atom<IGlobalFlag>({
  key: 'IglobalFlag',
  default: {
    joined: false,
    showLog: false,
  }
});

export const mainView = atom<LocalVideoTrack | RemoteVideoTrack>({
  key: 'ImainView',
  dangerouslyAllowMutability: true,
  default: null
});

type SmallViewTrackMap = Record<string, LocalVideoTrack | RemoteVideoTrack>

export const smallViewTrackMap = atom<SmallViewTrackMap>({
  key: 'IsmallViewTrackMap',
  dangerouslyAllowMutability: true,
  default: {},
})