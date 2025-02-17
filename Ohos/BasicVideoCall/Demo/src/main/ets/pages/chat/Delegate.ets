import {DingRtcConstants} from '@dingrtc/dingrtc'
export default interface Delegate {
  onJoin:(result: number, channel: string, userId: string, elapsed: number) => void
  onLeave:(result: number) => void
  onUserJoin:(uid: string) => void
  onUserLeave:(uid: string, reason: number) => void
  onChannelRemainingTimeNotify:(remainingTimeInSec: number) => void
  onBye:(code: DingRtcConstants.RtcEngineOnByeType) => void
  onConnectionStatusChanged: (status: DingRtcConstants.RtcEngineConnectionStatus,
    reason: DingRtcConstants.RtcEngineConnectionStatusChangeReason)=> void
  onNetworkQualityChanged: (uid: string,
    upQuality: DingRtcConstants.RtcEngineNetworkQuality, downQuality: DingRtcConstants.RtcEngineNetworkQuality)=>void
}