import {DingRtcSDK,DingRtc,DingRtcConstants,DingRtcEventListener} from '@dingrtc/dingrtc'
import * as model from './Model';
import Delegate from './Delegate'

let delegates: Array<Delegate> = []
let TAG: string = 'MultiChat'
let dingRtc:DingRtc | null;

export class ChatPresenter {

  private static instance: ChatPresenter = new ChatPresenter()
  private login?: model.LoginInfo


  private constructor() {
    delegates = []
  }

  static getInstance(): ChatPresenter {
    return ChatPresenter.instance;
  }

  addDelegate(delegate: Delegate) {

    if(delegates.indexOf(delegate) !== -1) {
      console.info(TAG, 'delegate is exits')
    } else {
      delegates?.push(delegate)
      console.info(TAG, 'setDelegate size: ' + delegates.length)
    }
  }

  removeDelegate(delegate: Delegate) {

    let index = delegates.indexOf(delegate)
    if(index !== -1) {
      delegates.splice(index, 1)
    } else {
      console.info(TAG, 'delegate not found.')
    }
  }

  createRtc(context: Context,level?: number, dir?: string): void {
    if(!dingRtc) {
      if(level)
        DingRtcSDK.setLogLevel(level)
      if(dir)
        DingRtcSDK.setLogDir(dir)
      dingRtc = DingRtcSDK.create(context);
      dingRtc.setEventListener(new ChatRtcEventListener());
    }

  }

  join(login: model.LoginInfo): number | undefined {
    console.info(TAG, "=== Invoke join Start. ===")
    this.login = login
    let ret:number | undefined = DingRtcConstants.ErrorCode.NoError
    if(login) {
      let authInfo = new DingRtcConstants.AuthInfo();
      authInfo.channelId = login.cname;
      authInfo.userId = login.uid;
      authInfo.appId = login.appId;
      authInfo.token = login.token;
      ret = dingRtc?.joinChannel(authInfo, login.uid)
      console.info(TAG, 'Join channel ret: ' + ret)
    } else {
      console.error(TAG, 'LoginInfo is empty.')
      ret = DingRtcConstants.ErrorCode.JoinBadParam
    }

    return ret
  }

  getUserInfo(uid: string): DingRtcConstants.UserInfo | undefined {
    let userInfo = dingRtc?.getUserInfo(uid);
    return userInfo;
  }

  pubAudio(pub: boolean): boolean {
    let ret = dingRtc?.publishLocalAudioStream(pub)
    console.info(TAG, `pubAudio ret:${ret}`)
    return ret == 0
  }

  pubVideo(pub: boolean): boolean {
    let ret = dingRtc?.publishLocalVideoStream(pub)
    console.info(TAG, `pubVideo ret:${ret}`)
    return ret == 0
  }

  setVideoEncodeConfig(width:number, height: number) {
    // let config: DingRtcConstants.RtcEngineVideoEncoderConfiguration =
    //   { width:width,height:height,frameRate:30,
    //     orientationMode:DingRtcConstants.RtcEngineVideoEncoderOrientationMode.RtcEngineVideoEncoderOrientationModeAdaptive,
    //     rotation: 0}
    let config = new DingRtcConstants.RtcEngineVideoEncoderConfiguration(width,height);
    dingRtc?.setVideoEncoderConfiguration(config);
  }

  changePreview(preview: boolean): boolean {
    let ret: number | undefined;
    if(preview) {
      ret = dingRtc?.startPreview();
    } else {
      ret = dingRtc?.stopPreview();
    }
    console.info(TAG, `changePreview ret:${ret}`)
    return ret == 0
  }

  enableVideo(enable: boolean): boolean {
    let ret = dingRtc?.enableLocalVideo(enable)
    console.info(TAG, `enableVideo ret:${ret}`)
    return ret == 0
  }

  switchCamera(): void {
    let ret = dingRtc?.switchCamera()
    console.info(TAG, `switchCamera ret:${ret}`)
  }

  muteAudio(mute: boolean): boolean {
    let ret = dingRtc?.muteLocalAudio(mute)
    console.info(TAG, `muteAudio ret:${ret}`)
    return ret == 0
  }

  muteVideo(mute: boolean): boolean {
    let ret = dingRtc?.muteLocalVideo(mute,DingRtcConstants.RtcEngineVideoTrack.RtcEngineVideoTrackCamera)
    console.info(TAG, `muteVideo ret:${ret}`)
    return ret == 0
  }

  setDefaultAudioRouteToSpeakerphone(speaker: boolean) {
    dingRtc?.setDefaultAudioRouteToSpeakerphone(speaker);
  }

  enableSpeakerphone(enable: boolean) {
    dingRtc?.enableSpeakerphone(enable);
  }

  leave() {
    let ret = dingRtc?.leaveChannel()
    console.info(TAG, `LeaveChannel ret:${ret}`)
  }

  attach(user: model.User): void {
    let canvas: DingRtcConstants.RtcEngineVideoCanvas = {xComponentId: user.uid}
    let isLocal = user.uid === this.login?.uid
    if(isLocal) {
      let ret = dingRtc?.setLocalViewConfig(canvas,DingRtcConstants.RtcEngineVideoTrack.RtcEngineVideoTrackCamera);
      console.info(TAG, `setLocalViewConfig ret:${ret}`)
    } else {
      let ret = dingRtc?.setRemoteViewConfig(canvas,user.uid,DingRtcConstants.RtcEngineVideoTrack.RtcEngineVideoTrackCamera);
      console.info(TAG, `setRemoteViewConfig ret:${ret}`)
    }
  }

  release() {
    dingRtc?.release()
    dingRtc = null
    console.info(TAG, 'Call release done.')
  }
}

class ChatRtcEventListener extends DingRtcEventListener {

  onJoinChannelResult(result: number, channel: string, userId: string, elapsed: number): void {
    console.info(TAG, `onJoinChannelResult: ${result}, elapsed: ${elapsed}`)
    if(result == DingRtcConstants.ErrorCode.NoError) {

    }
    delegates?.forEach(delegate => {
      delegate.onJoin(result, channel,userId,elapsed)
    })
  }

  onLeaveChannelResult(result: number, stats: DingRtcConstants.Stats): void {
    console.info(TAG, `onLeaveChannelResult: ${result}`)
    delegates?.forEach(delegate => {
      delegate.onLeave(result)
    })
  }

  onChannelRemainingTimeNotify(remainingTimeInSec: number):void {
    console.info(TAG, `onChannelRemainingTimeNotify: ${remainingTimeInSec}`)
    delegates?.forEach(delegate => {
      delegate.onChannelRemainingTimeNotify(remainingTimeInSec)
    })
  }

  onRemoteUserOnLineNotify(uid: string, elapsed: number): void {
    console.info(TAG, `User:${uid} joined.`)
    delegates?.forEach(delegate => {
      if(delegate.onUserJoin) {
        delegate.onUserJoin(uid)
      }
    })
  }

  onRemoteUserOffLineNotify(uid: string, reason: DingRtcConstants.UserOfflineReason): void {
    console.info(TAG, `User:${uid} leave.`)
    delegates?.forEach(delegate => {
      if(delegate.onUserLeave) {
        delegate.onUserLeave(uid, reason)
      }
    })
  }

  onRemoteTrackAvailableNotify(uid: string, audioTrack: DingRtcConstants.RtcEngineAudioTrack,
    videoTrack: DingRtcConstants.RtcEngineVideoTrack): void {

  }

  onBye(code: DingRtcConstants.RtcEngineOnByeType): void {
    console.info(TAG, `onBye:${code}.`)
    delegates?.forEach(delegate => {
      delegate.onBye(code)
    })
  };

  onConnectionStatusChanged(status: DingRtcConstants.RtcEngineConnectionStatus,
    reason: DingRtcConstants.RtcEngineConnectionStatusChangeReason): void {
    console.info(TAG, `onConnectionStatusChanged status: ${status} , reason: ${reason}}`)
    delegates?.forEach(delegate => {
      delegate.onConnectionStatusChanged(status,reason)
    })
  }

  onNetworkQualityChanged(uid: string,
    upQuality: DingRtcConstants.RtcEngineNetworkQuality, downQuality: DingRtcConstants.RtcEngineNetworkQuality): void {
    console.info(TAG, `onNetworkQualityChanged uid: ${uid} , upQuality: ${upQuality} , downQuality: ${downQuality}`)
    delegates?.forEach(delegate => {
      delegate.onNetworkQualityChanged(uid,upQuality,downQuality)
    })
  }

  onAudioSubscribeStateChanged(uid: string, oldState: DingRtcConstants.RtcEngineSubscribeState,
    newState: DingRtcConstants.RtcEngineSubscribeState, elapseSinceLastState: number, channel: string): void {

  }

  onVideoSubscribeStateChanged(uid: string, oldState: DingRtcConstants.RtcEngineSubscribeState,
    newState: DingRtcConstants.RtcEngineSubscribeState, elapseSinceLastState: number, channel: string): void {

  }

  onScreenShareSubscribeStateChanged(uid: string, oldState: DingRtcConstants.RtcEngineSubscribeState,
    newState: DingRtcConstants.RtcEngineSubscribeState, elapseSinceLastState: number, channel: string): void {

  }

  onSubscribeStreamTypeChanged(uid: string, oldStreamType: DingRtcConstants.RtcEngineVideoStreamType,
    newStreamType: DingRtcConstants.RtcEngineVideoStreamType, elapseSinceLastState: number, channel: string): void {

  }

  onUserAudioMuted(uid: string, isMute: boolean): void {

  }

  onUserVideoMuted(uid: string, isMute: boolean, videoTrack: DingRtcConstants.RtcEngineVideoTrack): void {

  }

  onUserVideoEnabled(uid: string, enabled: boolean): void {

  }

  onUserWillResignActive(uid: string): void {

  }

  onUserWillBecomeActive(uid: string): void {

  }

  onLocalAudioStats(localAudioStats: DingRtcConstants.LocalAudioStats): void {

  }


  onLocalVideoStats(localVideoStats: DingRtcConstants.LocalVideoStats): void {

  }


  onRemoteAudioStats(remoteAudioStats: DingRtcConstants.RemoteAudioStats): void {

  }


  onRemoteVideoStats(remoteVideoStats: DingRtcConstants.RemoteVideoStats): void {

  }

  onAudioRouteChanged(audioRouteType: DingRtcConstants.RtcEngineAudioRouteType): void {

  }

  onAudioVolumeIndication(speakers: Array<DingRtcConstants.RtcAudioVolumeInfo>, speakerNumber: number): void {

  }


}



