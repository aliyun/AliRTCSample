import {AliEngineErrorCode} from "../data/main"
export default class Error {
  constructor() {

  }
  static parseErrorCode(errorCode) {
    let errorMessage;
    switch (errorCode){
      case AliEngineErrorCode.AliEngineErrorJoinBadAppId                       :
        errorMessage = "AppId不存在"
        break;
      case AliEngineErrorCode.AliEngineErrorJoinInvaildAppId                   :
        errorMessage = "AppId已失效"
        break;
      case AliEngineErrorCode.AliEngineErrorJoinBadChannel                     :
        errorMessage = "频道不存在"
        break;
      case AliEngineErrorCode.AliEngineErrorJoinInvaildChannel                 :
        errorMessage = "频道已失效"
        break;
      case AliEngineErrorCode.AliEngineErrorJoinBadToken                       :
        errorMessage = "token不存在"
        break;
      case AliEngineErrorCode.AliEngineErrorJoinTimeout                        :
        errorMessage = "加入频道超时"
        break;
      case AliEngineErrorCode.AliEngineErrorJoinBadParam                       :
        errorMessage = "参数错误"
        break;
      case AliEngineErrorCode.AliEngineErrorMicOpenFail                        :
        errorMessage = "采集设备初始化失败"
        break;
      case AliEngineErrorCode.AliEngineErrorSpeakerOpenFail                    :
        errorMessage = "播放设备初始化失败"
        break;
      case AliEngineErrorCode.AliEngineErrorMicInterrupt                       :
        errorMessage = "采集过程中出现异常"
        break;
      case AliEngineErrorCode.AliEngineErrorSpeakerInterrupt                   :
        errorMessage = "播放过程中出现异常"
        break;
      case AliEngineErrorCode.AliEngineErrorMicAuthFail                        :
        errorMessage = "麦克风设备未授权"
        break;
      case AliEngineErrorCode.AliEngineErrorMicNotAvailable                    :
        errorMessage = "无可用的音频采集设备"
        break;
      case AliEngineErrorCode.AliEngineErrorSpeakerNotAvailable                :
        errorMessage = "无可用的音频播放设备"
        break;
      case AliEngineErrorCode.AliEngineErrorCameraOpenFail                     :
        errorMessage = "采集设备初始化失败"
        break;
      case AliEngineErrorCode.AliEngineErrorCameraInterrupt                    :
        errorMessage = "采集过程中出现异常"
        break;
      case AliEngineErrorCode.AliEngineErrorVideoDisplayOpenFail                    :
        errorMessage = "渲染设备初始化失败"
        break;
      case AliEngineErrorCode.AliEngineErrorVideoDisplayInterrupt                   :
        errorMessage = "渲染过程中出现异常"
        break;
      case AliEngineErrorCode.AliEngineErrorIceConnectionConnectFail           :
        errorMessage = "媒体通道建立失败"
        break;
      case AliEngineErrorCode.AliEngineErrorIceConnectionReconnectFail         :
        errorMessage = "媒体通道重连失败"
        break;
      case AliEngineErrorCode.AliEngineErrorIceConnectionHeartbeatTimeout      :
        errorMessage = "信令心跳超时"
        this.destroyEngine();
        break;
      case AliEngineErrorCode.AliEngineErrorLowLatencyLiveCommunicationFail         :
        errorMessage = "低延时互动直播信令失败"
        break;
      case AliEngineErrorCode.AliEngineErrorLowLatencyLiveMediaSettingFail          :
        errorMessage = "低延时互动直播媒体设置错误"
        break;
      case AliEngineErrorCode.AliEngineErrorLowLatencyLiveNoneBroadcaster             :
        errorMessage = "低延时互动直播当前房间没有主播推流"
        break;
      case AliEngineErrorCode.AliEngineErrorLowLatencyLiveNeedRestart                 :
        errorMessage = "低延时互动直播错误，应用层需要重新创建新的SDK实例"
        this.destroyEngine();
        break;
      case AliEngineErrorCode.AliEngineErrorLowLatencyLiveNeedRestartAgainLater       :
        errorMessage = "低延时互动直播错误，应用层需要稍等一定时间（自定义间隔）重新创建新的SDK实例"
        this.destroyEngine();
        break;
      case AliEngineErrorCode.AliEngineErrorLowLatencyLiveServiceUnavailable          :
        errorMessage = "低延时互动直播服务不可用或者服务降级"
        break;
      case AliEngineErrorCode.AliEngineErrorLowLatencyLiveMediaConnectionInterrruption :
        errorMessage = "低延时互动直播网络中断错误"
        break;
      case AliEngineErrorCode.AliEngineErrorInvaildState                    :
        errorMessage = "sdk状态错误"
        break;
      case AliEngineErrorCode.AliEngineErrorSessionRemoved                     :
        errorMessage = "Session已经被移除"
        this.destroyEngine();
        break;
      case AliEngineErrorCode.AliEngineErrorAudioBufferFull                    :
        errorMessage = "buffer队列饱和，用于外部输送裸数据功能"
        break;
      case AliEngineErrorCode.AliEngineErrorVideoBufferFull                    :
        errorMessage = "buffer队列饱和，用于外部输送裸数据功能"
        break;
      case AliEngineErrorCode.AliEngineErrorUpdateRoleChannel                  :
        errorMessage = "互动模式下设置角色错误"
        break;
      case AliEngineErrorCode.AliEngineErrorPublishInvaild             :
        errorMessage = "本地流（视频流、屏幕流、音频流）错误"
        break;
      case AliEngineErrorCode.AliEngineErrorSubscribeInvaild           :
        errorMessage = "远端流（视频流、屏幕流、音频流）错误"
        break;
      case AliEngineErrorCode.AliEngineErrorFailedToJoinChannelDueToInvaildChannelProfile:
        errorMessage = "检查用户入会模式和其他用户的入会模式（频道模式）是否一致"
        break;
      case AliEngineErrorCode.AliEngineErrorFailedToJoinChannelDueToAlreadyInChannel:
        errorMessage = "加入频道时，当前用户已经在频道中"
        break;
      case AliEngineErrorCode.AliEngineErrorFailedToLeaveChannelDueToAlreadyLeaved:
        errorMessage = "调用离开频道API时，当前用户已经在离会中了"
        break;
      case AliEngineErrorCode.AliEngineErrorFailedToUpdateRole                 :
        errorMessage = "更新角色失败"
        break;
      case AliEngineErrorCode.AliEngineErrorFailedToPublishDueToWrongRole      :
        errorMessage = "观众角色不能推流"
        break;
      case AliEngineErrorCode.AliEngineErrorInner                              :
        errorMessage = "其他错误"
        break;
      default                                                          :
        errorMessage = "none"
        break;
    }
    return errorMessage
  }

  static destroyEngine() {
    aliElectronRtc.leaveChannel();
    aliElectronRtc.destroy()
    aliElectronRtc = null
  }
}
