import DingRTC, {
  CameraVideoTrack,
  LocalAudioTrack,
  LocalVideoTrack,
  MicrophoneAudioTrack,
  RemoteAudioTrack,
  RemoteUser,
  VideoDimension,
  NetworkQuality,
  Group,
  RemoteVideoTrack,
  DingRTCClient,
  OptimizationMode,
} from 'dingrtc';
import { defineStore } from 'pinia';
import { isIOS, isMobile, isWeixin, logLevel, parseSearch } from './utils/tools';
import configJson from '~/config.json';

DingRTC.setLogLevel(logLevel);

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
  // encodeCameraLayers?: number;
  // encodeScreenLayers?: number;
  sendCameraLayers?: number;
  sendScreenLayers?: number;
  uplinkProfile?: string;
  downlinkProfile?: string;
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
  cameraMaxBitrate: number;
  cameraList: MediaDeviceInfo[];
  speakerList: MediaDeviceInfo[];
  micList: MediaDeviceInfo[];
  cameraOptimization: OptimizationMode,
  screenOptimization: OptimizationMode,
  facingMode: 'user' | 'environment';
}

interface TrackStats {
  mic?: boolean;
  camera?: boolean;
  screen?: boolean;
  hasCamera?: boolean;
  hasScreen?: boolean;
  subscribedCamera?: boolean;
  subscribedScreen?: boolean;
}

export interface IChannelInfo {
  mcuAudioTrack: RemoteAudioTrack;
  remoteUsers: RemoteUser[];
  speakers?: string[];
  subscribeAllVideo?: boolean;
  groups: Group[];
  subscribeAudio: string;
  cameraTrack: CameraVideoTrack;
  micTrack?: MicrophoneAudioTrack;
  screenTrack?: LocalVideoTrack;
  customVideoTrack?: LocalVideoTrack;
  customAudioTrack?: LocalAudioTrack;
  publishedTracks?: Set<string>;
  timeLeft?: number;
  networkQuality: NetworkQuality;
  rtcStats: RTCStats;
  defaultRemoteStreamType: string;
  remoteUserNetworkQualitys: { [key: string]: number };
  mainViewUserId: string;
  mainViewPreferType: 'camera' | 'auxiliary';
  mode: 'grid' | 'standard';
  trackStatsMap: Map<string, TrackStats>;
}

interface IGlobalFlag {
  joined: boolean;
  immersive: boolean;
  isMobile: boolean,
  hideLog: boolean,
  isIOS: boolean,
  isWeixin: boolean,
}

const client = DingRTC.createClient();

export const useClient = (): DingRTCClient => {
  const store = useInnerClientStore();
  return store.client;
};

export const useInnerClientStore: any = defineStore('ClientStore', {
  state: () => ({
    client,
  }),
});

export const useCurrentUserInfo = defineStore('ICurrentUserInfo', {
  state: () => ({
    appId: parseSearch('appId') || configJson.appId || '',
    userId: parseSearch('userId') || configJson.userId || `${Math.ceil(Math.random() * 10000)}`,
    userName:
      parseSearch('userName') || configJson.userName || `Web-${Math.ceil(Math.random() * 100)}`,
    channel:
      parseSearch('channelId') || configJson.channelId || `${Math.ceil(Math.random() * 10000)}`,
  }),
});

let defaultCameraDimension: VideoDimension = !isMobile() ? 'VD_1920x1080' : 'VD_640x480';
if (isIOS()) {
  defaultCameraDimension = 'VD_1280x720';
}

export const useDeviceInfo = defineStore('IDeviceInfo', {
  state: (): IDeviceInfo => ({
    cameraId: '',
    micId: '',
    speakerId: '',
    cameraList: [],
    speakerList: [],
    micList: [],
    cameraFrameRate: 15,
    cameraMaxBitrate: undefined,
    screenOptimization: 'detail',
    cameraOptimization: 'balanced',
    cameraDimension: defaultCameraDimension,
    screenFrameRate: 5,
    screenMaxBitrate: undefined,
    screenDimension: 'VD_1920x1080',
    facingMode: 'user',
  }),
  getters: {
    cameraEnable(): boolean {
      const channelInfo = useChannelInfo();
      return channelInfo.cameraTrack?.enabled && !channelInfo.cameraTrack?.muted;
    },
    micEnable(): boolean {
      const channelInfo = useChannelInfo();
      return channelInfo.micTrack?.enabled && !channelInfo.micTrack?.muted;
    }
  }
});

export const useChannelInfo = defineStore('IChannelInfo', {
  state: (): IChannelInfo => ({
    cameraTrack: null,
    screenTrack: null,
    customVideoTrack: null,
    customAudioTrack: null,
    micTrack: null,
    timeLeft: 0,
    defaultRemoteStreamType: 'FHD',
    networkQuality: 1,
    publishedTracks: new Set(),
    rtcStats: {},
    mode: 'grid',
    mcuAudioTrack: null,
    remoteUsers: [],
    groups: [],
    subscribeAllVideo: true,
    speakers: [],
    subscribeAudio: '',
    remoteUserNetworkQualitys: {},
    mainViewPreferType: 'auxiliary',
    mainViewUserId: '',
    trackStatsMap: new Map(),
  }),
  getters: {
    allUsers(): RemoteUser[] {
      const currentUserInfo = useCurrentUserInfo();
      const list: any = [
        {
          userId: currentUserInfo.userId,
          videoTrack: this.cameraTrack,
          userName: currentUserInfo.userName,
          audioMuted: !this.micTrack?.enabled,
          hasAudio: !!this.micTrack,
          audioTrack: this.micTrack,
          hasVideo: !!this.cameraTrack,
          videoMuted: !this.cameraTrack?.enabled,
          auxiliaryMuted: !this.screenTrack?.enabled,
          hasAuxiliary: !!this.screenTrack,
          auxiliaryTrack: this.screenTrack,
        },
        ...this.remoteUsers,
      ]
      return list as RemoteUser[];
    },
    mainViewUserInfo(): RemoteUser {
      return this.allUsers.find(user => user.userId === this.mainViewUserId)
    },
    mainViewTrack(): RemoteVideoTrack {
      return this.mainViewPreferType === 'camera' ? this.mainViewUserInfo?.videoTrack : this.mainViewUserInfo?.auxiliaryTrack
    },
    getTrack() {
      const self = this;
      return (user: RemoteUser): RemoteVideoTrack => {
        const videoTrack = user.videoTrack;
        const auxiliaryTrack = user.auxiliaryTrack;
        if (self.mainViewUserId === user.userId && self.mode === 'standard') {
          return self.mainViewPreferType === 'camera'
            ? auxiliaryTrack
            : videoTrack;
        }
        return auxiliaryTrack || videoTrack;
      }
    }
  },
  actions: {
    updateTrackStats(uid?: string) {
      const currentUserInfo = useCurrentUserInfo()
      const user = this.allUsers.find(item => item.userId === (uid || currentUserInfo.userId));
      this.trackStatsMap.set(user.userId, {
        mic: user.hasAudio && !user.audioMuted,
        screen: user.hasAuxiliary && !user.auxiliaryMuted,
        camera: user.hasVideo && !user.videoMuted,
        hasCamera: user.hasVideo,
        hasScreen: user.hasAuxiliary,
        subscribedCamera: !!user.videoTrack,
        subscribedScreen: !!user.auxiliaryTrack,
      });
    },
    updatePublishedTracks(trackIds: string[], action: 'add' | 'remove') {
      for (const trackId of trackIds) {
        if (action === 'add') {
          this.publishedTracks.add(trackId)
        } else {
          this.publishedTracks.delete(trackId)
        }
      }
    }
  }
});

export const useGlobalFlag = defineStore('IGlobalFlag', {
  state: (): IGlobalFlag => ({
    joined: false,
    immersive: false,
    isMobile: !!isMobile(),
    hideLog: logLevel === 'none',
    isIOS: !!isIOS(),
    isWeixin: !!isWeixin(),
  }),
});
