import { CameraVideoTrack, LocalVideoTrack } from 'dingrtc';

export interface ISmallViewProps {
  userId: string;
  videoTrack?: CameraVideoTrack;
  auxiliaryTrack?: LocalVideoTrack;
  userName: string;
  hasVideo: boolean;
  hasAudio: boolean;
  audioMuted: boolean;
  videoMuted: boolean;
  auxiliaryMuted: boolean;
  hasAuxiliary: boolean;
}
