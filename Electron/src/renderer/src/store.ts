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
  QualityReport,
  PlaybackAudioTrack,
} from 'dingrtc-electron-sdk';
import { defineStore } from 'pinia';
import configJson from '~/config.json';


DingRTC.setLogLevel('info');
//DingRTC.setLogDirPath('/Users/xxx/dd/electron/demo/logs/');


interface IDeviceInfo {
  cameraId: string;
  speakerId: string;
  micId: string;
  screenFrameRate: number;
  screenDimension: VideoDimension;
  cameraFrameRate: number;
  shareSystemAudio: boolean;
  cameraDimension: VideoDimension;
  cameraList: MediaDeviceInfo[];
  speakerList: MediaDeviceInfo[];
  cameraEnable: boolean;
  micEnable: boolean;
  micList: MediaDeviceInfo[];
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
  speakerTrack?: PlaybackAudioTrack;
  screenTrack?: LocalVideoTrack;
  customVideoTrack?: LocalVideoTrack;
  customAudioTrack?: LocalAudioTrack;
  publishedTracks?: Set<string>;
  timeLeft?: number;
  shareSourceId?: string;
  shareSourceType?: 'window' | 'screen';
  networkQuality: NetworkQuality;
  rtcStats: QualityReport;
  defaultRemoteStreamType: string;
  remoteUserNetworkQualitys: { [key: string]: number };
  mainViewUserId: string;
  mainViewPreferType: 'camera' | 'auxiliary';
  mode: 'grid' | 'standard';
  trackStatsMap: Map<string, TrackStats>;
  forceUpdateMainViewTrack: boolean;
}

interface IGlobalFlag {
  joined: boolean;
  immersive: boolean;
  env: string;
  dev: boolean;
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
    appId: configJson.appId || '',
    userId: configJson.userId || `${Math.ceil(Math.random() * 10000)}`,
    userName: configJson.userName || `Electron-${Math.ceil(Math.random() * 100)}`,
    channel: configJson.channelId || `${Math.ceil(Math.random() * 10000)}`,
  }),
});

let defaultCameraDimension: VideoDimension = 'VD_640x480';

export const useDeviceInfo = defineStore('IDeviceInfo', {
  state: (): IDeviceInfo => ({
    cameraId: '',
    micId: '',
    speakerId: '',
    cameraList: [],
    speakerList: [],
    micList: [],
    shareSystemAudio: false,
    cameraFrameRate: 15,
    cameraEnable: true,
    micEnable: true,
    cameraDimension: defaultCameraDimension,
    screenFrameRate: 5,
    screenDimension: 'VD_1920x1080',
  })
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
    networkQuality: 0,
    publishedTracks: new Set(),
    rtcStats: {},
    mode: 'grid',
    shareSourceType: 'window',
    mcuAudioTrack: null,
    remoteUsers: [],
    speakerTrack: null,
    shareSourceId: '',
    groups: [],
    subscribeAllVideo: true,
    speakers: [],
    subscribeAudio: '',
    remoteUserNetworkQualitys: {},
    mainViewPreferType: 'auxiliary',
    mainViewUserId: '',
    trackStatsMap: new Map(),
    forceUpdateMainViewTrack: false,
  }),
  getters: {
    allUsers(): RemoteUser[] {
      const currentUserInfo = useCurrentUserInfo();
      const deviceInfo = useDeviceInfo();
      const list: any = [
        {
          userId: currentUserInfo.userId,
          videoTrack: this.cameraTrack,
          userName: currentUserInfo.userName,
          audioMuted: !this.micTrack?.muted,
          hasAudio: !!this.micTrack,
          audioTrack: this.micTrack,
          hasVideo: !!this.cameraTrack,
          videoMuted: !deviceInfo.cameraEnable,
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
      if (this.forceUpdateMainViewTrack) {
        this.forceUpdateMainViewTrack = false;
      }
      const videoTrack = this.mainViewUserInfo?.videoTrack;
      const auxiliaryTrack = this.mainViewUserInfo?.auxiliaryTrack;
      return this.mainViewPreferType === 'camera' ? videoTrack || auxiliaryTrack : auxiliaryTrack || videoTrack;
    },
    getTrack() {
      const self = this;
      return (user: RemoteUser): RemoteVideoTrack => {
        const videoTrack = user.videoTrack;
        const auxiliaryTrack = user.auxiliaryTrack;
        if (self.mainViewUserId === user.userId && self.mode === 'standard') {
          return self.mainViewPreferType === 'camera'
            ? auxiliaryTrack || videoTrack
            : videoTrack || auxiliaryTrack;
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
    },
    updateUserNetworkQuality(uid: string, quality: number) {
      this.remoteUserNetworkQualitys[uid] = quality;
    }
  }
});

export const useGlobalFlag = defineStore('IGlobalFlag', {
  state: (): IGlobalFlag => ({
    joined: false,
    immersive: false,
    env: configJson.env || '',
    dev: process.env.NODE_ENV === 'development',
  }),
});
