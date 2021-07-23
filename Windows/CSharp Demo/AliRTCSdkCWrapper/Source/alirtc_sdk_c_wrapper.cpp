#include "engine_interface.h"
#include "alirtc_sdk_c_wrapper.h"
#include "error_codes.h"
#include "jsoncpp/json.h"
#include <mutex>

using namespace AliRTCSdk;

static AliEngine* _sharedEngine = nullptr;
static AliEngineEventListener* _sharedListener = nullptr;
static IAliEngineAudioDeviceManager* _AudioDeviceManager = nullptr;
static IAliEngineVideoDeviceManager* _VideoDeviceManager = nullptr;
static std::map <std::string, AliEngine*> _mapEngine;
static std::map <std::string, AliEngineEventListener*> _mapListener;
static std::map<std::string, IAliEngineMediaEngine*> _mapMediaEngine;
static std::map<std::string, IVideoFrameObserver*> _mapVideoFrameObserver;
static std::map<std::string, IAudioFrameObserver*> _mapAudioFrameObserver;
static std::string _cameraDeviceArray;
static std::string _audioCaptureDeviceArray;
static std::string _audioRenderDeviceArray;
static std::string _screenSourceArray;
static std::string _cameraNameId;
static std::string _audioCaptureNameId;
static std::string _audioRenderNameId;
static std::string _userListArray;
static std::mutex mMutexRecordUserStreams;
static std::map<std::string, bool> mMapRecordUserStreams;
static AliEngineRecordTemplate _RecordTemplate;
static AliEngineRecordVideoLayout _RecordVideoLayout;


static ALIRTC_FUNC_OnJoinChannelNotify _onJoinChannelNotify = nullptr;
static ALIRTC_FUNC_OnAudioPublishStateChangedNotify _onAudioPublishStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnVideoPublishStateChangedNotify _onVideoPublishStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnDualStreamPublishStateChangedNotify _onDualStreamPublishStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnScreenSharePublishStateChangedNotify _onScreenSharePublishStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnAudioSubscribeStateChangedNotify _onAudioSubscribeStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnVideoSubscribeStateChangedNotify _onVideoSubscribeStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnScreenShareSubscribeStateChangedNotify _onScreenShareSubscribeStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnSubscribeStreamTypeChangedNotify _onSubscribeStreamTypeChangedNotify = nullptr;

static ALIRTC_FUNC_OnRemoteUserOnLineNotify _onRemoteUserOnLineNotify = nullptr;
static ALIRTC_FUNC_OnRemoteUserOffLineNotify _onRemoteUserOffLineNotify = nullptr;
static ALIRTC_FUNC_OnRemoteTrackAvailableNotify _onRemoteTrackAvailableNotify = nullptr;
static ALIRTC_FUNC_OnNetworkQualityChangedNotify _onNetworkQualityChangedNotify = nullptr;
static ALIRTC_FUNC_OnByeNotify _onByeNotify = nullptr;
static ALIRTC_FUNC_OnOccurWarningNotify _onOccurWarningNotify = nullptr;
static ALIRTC_FUNC_OnOccurErrorNotify _onOccurErrorNotify = nullptr;
static ALIRTC_FUNC_OnAudioDeviceStateChangedNotify _onAudioDeviceStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnVideoDeviceStateChangedNotify _onVideoDeviceStateChangedNotify = nullptr;
static ALIRTC_FUNC_OnConnectionLostNotify _onConnectionLostNotify = nullptr;
static ALIRTC_FUNC_OnTryToReconnectNotify _onTryToReconnectNotify = nullptr;
static ALIRTC_FUNC_OnConnectionRecoveryNotify _onConnectionRecoveryNotify = nullptr;
static ALIRTC_FUNC_OnLeaveChannelResultNotify _onLeaveChannelResultNotify = nullptr;
static ALIRTC_FUNC_OnUpdateRoleNotify _onUpdateRoleNotify = nullptr;
static ALIRTC_FUNC_OnAudioVolumeCallback _onAudioVolumeCallbackNotify = nullptr;
static ALIRTC_FUNC_OnAudioDeviceRecordLevel _onAudioDeviceRecordLevelNotify = nullptr;
static ALIRTC_FUNC_OnAudioDevicePlayoutLevel _onAudioDevicePlayoutLevelNotify = nullptr;
static ALIRTC_FUNC_OnAudioDevicePlayoutEnd _onAudioDevicePlayoutEndNotify = nullptr;
static ALIRTC_FUNC_OnLastmileDetectResultWithQuality _onLastmileDetectResultWithQualityNotify = nullptr;
static ALIRTC_FUNC_OnMediaRecordEvent _onMediaRecordEventNotify = nullptr;

static ALIRTC_FUNC_OnRawDataAudioFrame _onRawDataAudioFrameNotify = nullptr;
static ALIRTC_FUNC_OnPublishAudioFrame _onPublishAudioFrameNotify = nullptr;
static ALIRTC_FUNC_OnSubscribeAudioFrame _onSubscribeAudioFrameNotify = nullptr;
static ALIRTC_FUNC_OnPlaybackAudioFrameBeforeMixing _onPlaybackAudioFrameBeforeMixingNotify = nullptr;

static ALIRTC_FUNC_OnCaptureVideoSample _onCaptureVideoSampleNotify = nullptr;
static ALIRTC_FUNC_OnPreEncodeVideoSample _onPreEncodeVideoSampleNotify = nullptr;
static ALIRTC_FUNC_OnRemoteVideoVideoSample _onRemoteVideoVideoSampleNotify = nullptr;

AliEngine* GetEngine(const char* channel)
{
  AliEngine* pEngine = _sharedEngine;
  auto iter = _mapEngine.find(channel);
  if (iter != _mapEngine.end())
    pEngine = iter->second;

  return pEngine;
}

IAliEngineMediaEngine* GetMediaEngine(const char* channel)
{
  auto iter = _mapMediaEngine.find(channel);
  if (iter != _mapMediaEngine.end())
    return iter->second;
  else
    return nullptr;
}
//////////////////////////////////////////////////////////////////////////
class AliRtcEngineEventListener : public AliEngineEventListener
{
public:
  AliRtcEngineEventListener(const String sChannelId);
  virtual void OnJoinChannelResult(int result, const char *channel, int elapsed) override;
  virtual void OnRemoteUserOnLineNotify(const char *uid, int elapsed) override;

  virtual void OnRemoteUserOffLineNotify(const char *uid, AliEngineUserOfflineReason reason) override;

  virtual void OnAudioPublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel) override;
  virtual void OnVideoPublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel) override;
  virtual void OnDualStreamPublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel) override;
  virtual void OnScreenSharePublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel) override;
  virtual void OnAudioSubscribeStateChanged(const char *uid,
    AliEngineSubscribeState oldState,
    AliEngineSubscribeState newState,
    int elapseSinceLastState,
    const char *channel) override;
  virtual void OnVideoSubscribeStateChanged(const char *uid,
    AliEngineSubscribeState oldState,
    AliEngineSubscribeState newState,
    int elapseSinceLastState,
    const char *channel) override;
  virtual void OnScreenShareSubscribeStateChanged(const char *uid,
    AliEngineSubscribeState oldState,
    AliEngineSubscribeState newState,
    int elapseSinceLastState,
    const char *channel) override;
  virtual void OnSubscribeStreamTypeChanged(const char *uid,
    AliEngineVideoStreamType oldStreamType,
    AliEngineVideoStreamType newStreamType,
    int elapseSinceLastState,
    const char *channel) override;

  virtual void OnRemoteTrackAvailableNotify(const char *uid,
    AliEngineAudioTrack audioTrack,
    AliEngineVideoTrack videoTrack) override;

  virtual void OnNetworkQualityChanged(const char *uid,
    AliEngineNetworkQuality upQuality,
    AliEngineNetworkQuality downQuality) override;

  virtual void OnBye(int code) override;

  virtual void OnOccurWarning(int warn, const char *msg) override;

  virtual void OnOccurError(int error, const char *msg) override;

  virtual void OnAudioDeviceStateChanged(const AliEngineDeviceInfo& deviceInfo, AliEngineExternalDeviceType deviceType, AliEngineExternalDeviceState deviceState) override;
  virtual void OnVideoDeviceStateChanged(const AliEngineDeviceInfo& deviceInfo, AliEngineExternalDeviceType deviceType, AliEngineExternalDeviceState deviceState) override;

  virtual void OnAudioDeviceRecordLevel(int level) override;
  virtual void OnAudioDevicePlayoutLevel(int level) override;
  virtual void OnAudioDevicePlayoutEnd() override;

  virtual void OnConnectionLost() override;

  virtual void OnTryToReconnect() override;

  virtual void OnConnectionRecovery() override;

  virtual void OnUpdateRoleNotify(const AliEngineClientRole old_role, const AliEngineClientRole new_role) override;

  virtual void OnLastmileDetectResultWithQuality(AliEngineNetworkQuality networkQuality) override;

  virtual void OnMediaRecordEvent(int event, const char* filePath) override;

  virtual void OnLeaveChannelResult(int result, AliEngineStats stats) override;

  virtual void OnAudioVolumeCallback(const AliEngineUserVolumeInfo* volumeInfo, int volumeInfoCount, int totalVolume) override;

private:
  String msChannelId;
};

AliRtcEngineEventListener::AliRtcEngineEventListener(const String sChannelId)
{
  msChannelId = sChannelId;
}

void AliRtcEngineEventListener::OnJoinChannelResult(int result, const char *channel, int elapsed)
{
  if (_onJoinChannelNotify)
  {
    if (0 == result)
    {
      if (msChannelId == "")
        msChannelId = channel;

      auto iter = _mapMediaEngine.find(msChannelId.c_str());
      if (iter == _mapMediaEngine.end())
      {
        IAliEngineMediaEngine* pMediaEngine = nullptr;
        AliEngine* pEngine = GetEngine(msChannelId.c_str());
        if (pEngine)
        {
          QUERY_INTERFACE(pEngine, AliEngineInterfaceMediaEngine, pMediaEngine);
          if (pMediaEngine)
            _mapMediaEngine[msChannelId.c_str()] = pMediaEngine;
        }
      }
    }

    _onJoinChannelNotify(result, channel, elapsed);
  }
}

void AliRtcEngineEventListener::OnLeaveChannelResult(int result, AliEngineStats stats)
{
  if (_onLeaveChannelResultNotify)
  {
    auto iter = _mapMediaEngine.find(msChannelId.c_str());
    if (iter != _mapMediaEngine.end())
    {
      if (iter->second)
      {
        iter->second->Release();
      }
      _mapMediaEngine.erase(msChannelId.c_str());
    }

    AliRTCStats rtcStats;
    rtcStats.appCpu = stats.appCpu;
    rtcStats.callDuration = stats.callDuration;
    rtcStats.lastmileDelay = stats.lastmileDelay;
    rtcStats.rcvdBytes = stats.rcvdBytes;
    rtcStats.rcvdExpectedPkts = stats.rcvdExpectedPkts;
    rtcStats.rcvdKBitrate = stats.rcvdKBitrate;
    rtcStats.rcvdLossPkts = stats.rcvdLossPkts;
    rtcStats.rcvdLossRate = stats.rcvdLossRate;
    rtcStats.sentBytes = stats.sentBytes;
    rtcStats.sentKBitrate = stats.sentKBitrate;
    rtcStats.sentLossRate = stats.sentLossRate;
    rtcStats.systemCpu = stats.systemCpu;
    rtcStats.sentExpectedPkts = stats.sentExpectedPkts;
    rtcStats.sentLossPkts = stats.sentLossPkts;
    rtcStats.videoRcvdKBitrate = stats.videoRcvdKBitrate;
    rtcStats.videoSentKBitrate = stats.videoSentKBitrate;
    _onLeaveChannelResultNotify(result, rtcStats,msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnRemoteUserOnLineNotify(const char *uid, int elapsed)
{
  if (_onRemoteUserOnLineNotify)
  {
    _onRemoteUserOnLineNotify(uid, elapsed, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnRemoteUserOffLineNotify(const char *uid, AliEngineUserOfflineReason reason)
{
  if (_onRemoteUserOffLineNotify)
  {
    _onRemoteUserOffLineNotify(uid, (AliRTCUserOfflineReason)reason, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnAudioPublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel)
{
  if (_onAudioPublishStateChangedNotify)
  {
    _onAudioPublishStateChangedNotify((AliRTCPublishState)oldState, (AliRTCPublishState)newState, elapseSinceLastState, channel);
  }
}

void AliRtcEngineEventListener::OnVideoPublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel)
{
  if (_onVideoPublishStateChangedNotify)
  {
    _onVideoPublishStateChangedNotify((AliRTCPublishState)oldState, (AliRTCPublishState)newState, elapseSinceLastState, channel);
  }
}

void AliRtcEngineEventListener::OnDualStreamPublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel)
{
  if (_onDualStreamPublishStateChangedNotify)
  {
    _onDualStreamPublishStateChangedNotify((AliRTCPublishState)oldState, (AliRTCPublishState)newState, elapseSinceLastState, channel);
  }
}

void AliRtcEngineEventListener::OnScreenSharePublishStateChanged(AliEnginePublishState oldState, AliEnginePublishState newState, int elapseSinceLastState, const char *channel)
{
  if (_onScreenSharePublishStateChangedNotify)
  {
    _onScreenSharePublishStateChangedNotify((AliRTCPublishState)oldState, (AliRTCPublishState)newState, elapseSinceLastState, channel);
  }
}

void AliRtcEngineEventListener::OnAudioSubscribeStateChanged(const char *uid,
  AliEngineSubscribeState oldState,
  AliEngineSubscribeState newState,
  int elapseSinceLastState,
  const char *channel)
{
  if (_onAudioSubscribeStateChangedNotify)
  {
    _onAudioSubscribeStateChangedNotify(uid, (AliRTCSubscribeState)oldState, (AliRTCSubscribeState)newState, elapseSinceLastState, channel);
  }
}
void AliRtcEngineEventListener::OnVideoSubscribeStateChanged(const char *uid,
  AliEngineSubscribeState oldState,
  AliEngineSubscribeState newState,
  int elapseSinceLastState,
  const char *channel)
{
  if (_onVideoSubscribeStateChangedNotify)
  {
    _onVideoSubscribeStateChangedNotify(uid, (AliRTCSubscribeState)oldState, (AliRTCSubscribeState)newState, elapseSinceLastState, channel);
  }
}
void AliRtcEngineEventListener::OnScreenShareSubscribeStateChanged(const char *uid,
  AliEngineSubscribeState oldState,
  AliEngineSubscribeState newState,
  int elapseSinceLastState,
  const char *channel)
{
  if (_onScreenShareSubscribeStateChangedNotify)
  {
    _onScreenShareSubscribeStateChangedNotify(uid, (AliRTCSubscribeState)oldState, (AliRTCSubscribeState)newState, elapseSinceLastState, channel);
  }
}
void AliRtcEngineEventListener::OnSubscribeStreamTypeChanged(const char *uid,
  AliEngineVideoStreamType oldStreamType,
  AliEngineVideoStreamType newStreamType,
  int elapseSinceLastState,
  const char *channel)
{
  if (_onSubscribeStreamTypeChangedNotify)
  {
    _onSubscribeStreamTypeChangedNotify(uid, (AliRTCVideoStreamType)oldStreamType, (AliRTCVideoStreamType)newStreamType, elapseSinceLastState, channel);
  }
}

void AliRtcEngineEventListener::OnRemoteTrackAvailableNotify(const char *uid,
  AliEngineAudioTrack audioTrack,
  AliEngineVideoTrack videoTrack)
{
  if (_onRemoteTrackAvailableNotify)
  {
    _onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnNetworkQualityChanged(const char *uid, AliEngineNetworkQuality upQuality, AliEngineNetworkQuality downQuality)
{
  if (_onNetworkQualityChangedNotify)
  {
    _onNetworkQualityChangedNotify(uid, (AliRTCNetworkQuality)upQuality, (AliRTCNetworkQuality)downQuality, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnBye(int code)
{
  if (_onByeNotify)
  {
    _onByeNotify(code, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnOccurWarning(int warn, const char *msg)
{
  if (_onOccurWarningNotify)
  {
    _onOccurWarningNotify(warn, msg, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnOccurError(int error, const char *msg)
{
  if (_onOccurErrorNotify)
  {
    _onOccurErrorNotify(error, msg, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnAudioDeviceStateChanged(const AliEngineDeviceInfo& deviceInfo, AliEngineExternalDeviceType deviceType, AliEngineExternalDeviceState deviceState)
{
  if (_onAudioDeviceStateChangedNotify)
  {
    AliRTCDeviceInfo info;
    info.deviceName = deviceInfo.deviceName.c_str();
    info.deviceID = deviceInfo.deviceID.c_str();
    _onAudioDeviceStateChangedNotify(info, deviceType, deviceState, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnVideoDeviceStateChanged(const AliEngineDeviceInfo& deviceInfo, AliEngineExternalDeviceType deviceType, AliEngineExternalDeviceState deviceState)
{
  if (_onVideoDeviceStateChangedNotify)
  {
    AliRTCDeviceInfo info;
    info.deviceName = deviceInfo.deviceName.c_str();
    info.deviceID = deviceInfo.deviceID.c_str();
    _onVideoDeviceStateChangedNotify(info, deviceType, deviceState, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnConnectionLost()
{
  if (_onConnectionLostNotify)
  {
    _onConnectionLostNotify(msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnTryToReconnect()
{
  if (_onTryToReconnectNotify)
  {
    _onTryToReconnectNotify(msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnConnectionRecovery()
{
  if (_onConnectionRecoveryNotify)
  {
    _onConnectionRecoveryNotify(msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnUpdateRoleNotify(const AliEngineClientRole old_role, const AliEngineClientRole new_role)
{
  if (_onUpdateRoleNotify)
  {
    _onUpdateRoleNotify(old_role, new_role, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnAudioVolumeCallback(const AliEngineUserVolumeInfo* volumeInfo, int volumeInfoCount, int totalVolume)
{
  if (_onAudioVolumeCallbackNotify)
  {
    AliRTCUserVolumeInfo info;
    info.speechState = volumeInfo->speechState;
    info.sumVolume = volumeInfo->sumVolume;
    info.volume = volumeInfo->volume;
    info.uid = volumeInfo->uid.c_str();
    _onAudioVolumeCallbackNotify(&info, volumeInfoCount, totalVolume, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnLastmileDetectResultWithQuality(AliEngineNetworkQuality networkQuality)
{
  if (_onLastmileDetectResultWithQualityNotify)
  {
    _onLastmileDetectResultWithQualityNotify((AliRTCNetworkQuality)networkQuality, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnMediaRecordEvent(int event, const char* filePath)
{
  if (_onMediaRecordEventNotify)
  {
    _onMediaRecordEventNotify(event, filePath);
  }
}

void AliRtcEngineEventListener::OnAudioDeviceRecordLevel(int level)
{
  if (_onAudioDeviceRecordLevelNotify)
  {
    _onAudioDeviceRecordLevelNotify(level, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnAudioDevicePlayoutLevel(int level)
{
  if (_onAudioDevicePlayoutLevelNotify)
  {
    _onAudioDevicePlayoutLevelNotify(level, msChannelId.c_str());
  }
}

void AliRtcEngineEventListener::OnAudioDevicePlayoutEnd()
{
  if (_onAudioDevicePlayoutEndNotify)
  {
    _onAudioDevicePlayoutEndNotify(msChannelId.c_str());
  }
}

class AliRtcAudioFrameObserver : public IAudioFrameObserver
{
public:
  AliRtcAudioFrameObserver(String& sChannelId) { msChannelId = sChannelId; }

  virtual bool OnRawDataAudioFrame(AliEngineAudioRawData audioRawData) override
  {
    if (_onRawDataAudioFrameNotify)
    {
      AliRTCAudioRawData data;
      data.bytesPerSample = audioRawData.bytesPerSample;
      data.dataPtr = audioRawData.dataPtr;
      data.numOfChannels = audioRawData.numOfChannels;
      data.numOfSamples = audioRawData.numOfSamples;
      data.sampleRate = audioRawData.sampleRate;
      data.samplesPerSec = audioRawData.samplesPerSec;
      _onRawDataAudioFrameNotify(data, msChannelId.c_str());
      return true;
    }
    return false;
  }

  virtual bool OnPublishAudioFrame(AliEngineAudioRawData audioRawData) override
  {
    if (_onPublishAudioFrameNotify)
    {
      AliRTCAudioRawData data;
      data.bytesPerSample = audioRawData.bytesPerSample;
      data.dataPtr = audioRawData.dataPtr;
      data.numOfChannels = audioRawData.numOfChannels;
      data.numOfSamples = audioRawData.numOfSamples;
      data.sampleRate = audioRawData.sampleRate;
      data.samplesPerSec = audioRawData.samplesPerSec;
      _onPublishAudioFrameNotify(data, msChannelId.c_str());
      return true;
    }
    return false;
  }

  virtual bool OnSubscribeAudioFrame(AliEngineAudioRawData audioRawData) override
  {
    if (_onSubscribeAudioFrameNotify)
    {
      AliRTCAudioRawData data;
      data.bytesPerSample = audioRawData.bytesPerSample;
      data.dataPtr = audioRawData.dataPtr;
      data.numOfChannels = audioRawData.numOfChannels;
      data.numOfSamples = audioRawData.numOfSamples;
      data.sampleRate = audioRawData.sampleRate;
      data.samplesPerSec = audioRawData.samplesPerSec;
      _onSubscribeAudioFrameNotify(data, msChannelId.c_str());
      return true;
    }
    return false;
  }

  virtual bool OnPlaybackAudioFrameBeforeMixing(const char *uid, AliEngineAudioRawData audioRawData) override
  {
    if (_onPlaybackAudioFrameBeforeMixingNotify)
    {
      AliRTCAudioRawData data;
      data.bytesPerSample = audioRawData.bytesPerSample;
      data.dataPtr = audioRawData.dataPtr;
      data.numOfChannels = audioRawData.numOfChannels;
      data.numOfSamples = audioRawData.numOfSamples;
      data.sampleRate = audioRawData.sampleRate;
      data.samplesPerSec = audioRawData.samplesPerSec;
      _onPlaybackAudioFrameBeforeMixingNotify(uid, data, msChannelId.c_str());
      return true;
    }
    return false;
  }

protected:
  String msChannelId;
};

class AliRtcVideoFrameObserver : public IVideoFrameObserver
{
public:
  AliRtcVideoFrameObserver(String& sChannelId) { msChannelId = sChannelId; }

  virtual bool OnCaptureVideoSample(AliEngineVideoSource videoSource, AliEngineVideoRawData &videoRawData) override
  {
    if (_onCaptureVideoSampleNotify)
    {
      AliRTCVideoDataSample frame;
      frame.dataPtr = videoRawData.dataPtr;
      frame.dataLength = videoRawData.dataLength;
      frame.format = (AliRTCVideoFormat)videoRawData.format;
      frame.bufferType = (AliRTCBufferType)videoRawData.type;
      frame.height = videoRawData.height;
      frame.width = videoRawData.width;
      frame.rotation = videoRawData.rotation;
      frame.strideY = videoRawData.strideY;
      frame.strideU = videoRawData.strideU;
      frame.strideV = videoRawData.strideV;
      frame.timeStamp = videoRawData.timeStamp;
      _onCaptureVideoSampleNotify((AliRTCVideoSource)videoSource, &frame, msChannelId.c_str());
      return true;
    }
    return false;
  }

  virtual bool OnPreEncodeVideoSample(AliEngineVideoSource videoSource, AliEngineVideoRawData &videoRawData) override
  {
    if (_onPreEncodeVideoSampleNotify)
    {
      AliRTCVideoDataSample frame;
      frame.dataPtr = videoRawData.dataPtr;
      frame.dataLength = videoRawData.dataLength;
      frame.format = (AliRTCVideoFormat)videoRawData.format;
      frame.bufferType = (AliRTCBufferType)videoRawData.type;
      frame.height = videoRawData.height;
      frame.width = videoRawData.width;
      frame.rotation = videoRawData.rotation;
      frame.strideY = videoRawData.strideY;
      frame.strideU = videoRawData.strideU;
      frame.strideV = videoRawData.strideV;
      frame.timeStamp = videoRawData.timeStamp;
      _onPreEncodeVideoSampleNotify((AliRTCVideoSource)videoSource, &frame, msChannelId.c_str());
      return true;
    }
    return false;
  }

  virtual bool OnRemoteVideoSample(const char *uid, AliEngineVideoSource videoSource, AliEngineVideoRawData &videoRawData) override
  {
    if (_onRemoteVideoVideoSampleNotify)
    {
      AliRTCVideoDataSample frame;
      frame.dataPtr = videoRawData.dataPtr;
      frame.dataLength = videoRawData.dataLength;
      frame.format = (AliRTCVideoFormat)videoRawData.format;
      frame.bufferType = (AliRTCBufferType)videoRawData.type;
      frame.height = videoRawData.height;
      frame.width = videoRawData.width;
      frame.rotation = videoRawData.rotation;
      frame.strideY = videoRawData.strideY;
      frame.strideU = videoRawData.strideU;
      frame.strideV = videoRawData.strideV;
      frame.timeStamp = videoRawData.timeStamp;
      _onRemoteVideoVideoSampleNotify(uid, (AliRTCVideoSource)videoSource, &frame, msChannelId.c_str());
      return true;
    }
    return false;
  }

  virtual AliEngineVideoFormat GetVideoFormatPreference() override
  { 
    return AliEngineVideoFormatI420; 
  }

  virtual uint32_t GetObservedFramePosition() override
  { 
    return static_cast<uint32_t>(AliEnginePositionPostCapture | AliEnginePositionPreRender | AliEnginePositionPreEncoder);
  };

protected:
  String msChannelId;
};

//////////////////////////////////////////////////////////////////////////
ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcCreateEngine(const char* extras)
{
  if (nullptr == _sharedListener)
    _sharedListener = new AliRtcEngineEventListener("");

  if (nullptr == _sharedEngine)
    _sharedEngine = AliEngine::Create(extras);

  if (_sharedEngine)
  {
    _sharedEngine->SetEngineEventListener(_sharedListener);
    QUERY_INTERFACE(_sharedEngine, AliEngineInterfaceAudioDeviceManger, _AudioDeviceManager);
    QUERY_INTERFACE(_sharedEngine, AliEngineInterfaceVideoDeviceManger, _VideoDeviceManager);
  }
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcDestroyEngine()
{
  for (auto iter = _mapEngine.begin(); iter != _mapEngine.end(); ++iter)
  {
    if (_sharedEngine != iter->second)
      iter->second->DestroyChannel();
  }
  _mapEngine.clear();

  for (auto iter = _mapListener.begin(); iter != _mapListener.end(); ++iter)
  {
    if (iter->second)
      delete iter->second;
  }
  _mapListener.clear();

  for (auto iter = _mapMediaEngine.begin(); iter != _mapMediaEngine.end(); ++iter)
  {
    if (iter->second)
      iter->second->Release();
  }
  _mapMediaEngine.clear();

  if (_AudioDeviceManager)
  {
    _AudioDeviceManager->Release();
    _AudioDeviceManager = nullptr;
  }

  if (_VideoDeviceManager)
  {
    _VideoDeviceManager->Release();
    _VideoDeviceManager = nullptr;
  }

  if (_sharedEngine) {
    if (_sharedListener)
    {
      delete _sharedListener;
      _sharedListener = nullptr;
    }

    AliEngine::Destroy();
    _sharedEngine = nullptr;
  }
}

ALIRTCSDK_API const char * ALIRTCSDK_CALL AliRtcGetSdkVersion()
{
  return AliEngine::GetSDKVersion();
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStartLastmileDetect()
{
  if (_sharedEngine)
  {
    _sharedEngine->StartLastmileDetect();
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStopLastmileDetect()
{
  if (_sharedEngine)
  {
    _sharedEngine->StopLastmileDetect();
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcPublishLocalVideoStream(bool enabled, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->PublishLocalVideoStream(enabled);
  return -1;
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsLocalVideoStreamPublished(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->IsLocalVideoStreamPublished();
  return false;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcPublishLocalAudioStream(bool enabled, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->PublishLocalAudioStream(enabled);
  return -1;
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsLocalAudioStreamPublished(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->IsLocalAudioStreamPublished();
  return false;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcPublishLocalDualStream(bool enabled, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->PublishLocalDualStream(enabled);
  return -1;
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsDualStreamPublished(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->IsDualStreamPublished();
  return false;
}


ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetRemoteVideoStreamType(const char* uid, int streamType, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SetRemoteVideoStreamType(uid, (AliEngineVideoStreamType)streamType);
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetRemoteDefaultVideoStreamType(int streamType, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SetRemoteDefaultVideoStreamType((AliEngineVideoStreamType)streamType);
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetDefaultSubscribeAllRemoteAudioStreams(bool sub, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SetDefaultSubscribeAllRemoteAudioStreams(sub);
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSubscribeAllRemoteAudioStreams(bool sub, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SubscribeAllRemoteAudioStreams(sub);
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSubscribeRemoteAudioStream(const char* uid, bool sub, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SubscribeRemoteAudioStream(uid,sub);
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetDefaultSubscribeAllRemoteVideoStreams(bool sub, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SetDefaultSubscribeAllRemoteVideoStreams(sub);
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSubscribeAllRemoteVideoStreams(bool sub, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SubscribeAllRemoteVideoStreams(sub);
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSubscribeRemoteVideoStream(const char* uid, AliRTCVideoTrack track, bool sub, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SubscribeRemoteVideoStream(uid, (AliEngineVideoTrack)track, sub);
  return -1;
}


ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetAudioOnlyMode(bool audioOnly, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->SetAudioOnlyMode(audioOnly);
  return -1;
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsAudioOnlyMode(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->IsAudioOnlyMode();
  return false;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcJoinChannel(const char* channel, const char* userId, const char* appId, const char* nonce, const char* token, const char* role, const char* gslbArray, const char* agentArray, long long timestamp, const char* userName)
{
  int nRet = -1;
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    if (appId == nullptr || channel == nullptr || nonce == nullptr
      || token == nullptr || userId == nullptr)
    {
      return nRet;
    }

    AliEngineAuthInfo auth_info;
    auth_info.appId = (char*)appId;
    auth_info.channelId = (char*)channel;
    auth_info.nonce = (char*)nonce;
    auth_info.timestamp = timestamp;
    auth_info.token = (char*)token;
    auth_info.userId = (char*)userId;
    auth_info.role = (char*)role;

    //parse gslb array
    Json::Value gslbList;
    Json::Reader jsonReader;
    if (gslbArray != nullptr)
    {
      if (!jsonReader.parse(gslbArray, gslbList) || !gslbList.isArray())
      {
        auth_info.gslbCount = 1;
        auth_info.gslb = new char*[auth_info.gslbCount];
        auth_info.gslb[0] = (char*)gslbArray;
      }
      else
      {
        auth_info.gslbCount = gslbList.size();
        auth_info.gslb = new char*[auth_info.gslbCount];
        for (int i = 0; i < (int)auth_info.gslbCount; i++) {
          auth_info.gslb[i] = (char*)gslbList[i].asString().c_str();
        }
      }
    }

    //parse agent array
    Json::Value agentList;
    if (agentArray != nullptr)
    {
      if (!jsonReader.parse(agentArray, agentList) || !agentList.isArray())
      {
        auth_info.agentCount = 1;
        auth_info.agent = new char*[auth_info.agentCount];
        auth_info.agent[0] = (char*)agentArray;
      }
      else {
        auth_info.agentCount = agentList.size();
        auth_info.agent = new char*[auth_info.agentCount];
        for (int i = 0; i < (int)auth_info.agentCount; i++) {
          auth_info.agent[i] = (char*)agentList[i].asString().c_str();
        }
      }
    }

    nRet = pEngine->JoinChannel(auth_info, userName);
    delete[] auth_info.gslb;
    delete[] auth_info.agent;
  }
  return nRet;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcLeaveChannel(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->LeaveChannel();
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStartPreview()
{
  if (_sharedEngine)
  {
    return _sharedEngine->StartPreview();
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStopPreview()
{
  if (_sharedEngine)
  {
    return _sharedEngine->StopPreview();
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcMuteLocalCamera(bool mute, int track, const char *channel)
{
  if (track < AliEngineVideoTrackNo || track >= AliEngineVideoTrackBoth)
  {
    return -1;
  }

  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->MuteLocalCamera(mute, (AliEngineVideoTrack)track);
  }

  return -1;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetVideoEncoderConfiguration(const AliRTCVideoEncoderConfiguration& config)
{
  if (_sharedEngine)
  {
    AliEngineVideoEncoderConfiguration conf;
    conf.bitrate = config.bitrate;
    conf.dimensions.height = config.height;
    conf.dimensions.width = config.width;
    conf.frameRate = (AliEngineFrameRate)config.frameRate;
    conf.mirrorMode = (AliEngineVideoMirrorMode)config.mirrorMode;
    conf.orientationMode = (AliEngineVideoEncoderOrientationMode)config.orientationMode;
    conf.rotationMode = (AliEngineRotationMode)config.rotationMode;
    _sharedEngine->SetVideoEncoderConfiguration(conf);
  }
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcMuteLocalMic(bool mute, AliRTCMuteLocalAudioMode mode, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->MuteLocalMic(mute, (AliEngineMuteLocalAudioMode)mode);
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcMuteRemoteAudio(const char* userId, bool mute, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->MuteRemoteAudio(userId, mute);
  }

  return -1;
}


ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcMuteAllRemoteAudio(bool mute, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->MuteAllRemoteAudio(mute);
  }

  return -1;
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetOnlineRemoteUsers(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine == nullptr)
  {
    return "";
  }

  StringArray userList;
  pEngine->GetOnlineRemoteUsers(userList);
  if (userList.size() <= 0)
  {
    return "";
  }

  Json::Value arrayObj;
  for (int i = 0; i < userList.size(); i++)
  {
    arrayObj.append(userList.at(i).c_str());
  }

  _userListArray = arrayObj.toStyledString();
  return _userListArray.c_str();
}


ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetAudioCaptures()
{
  if (_AudioDeviceManager)
  {
    AliEngineDeviceInfoList* pList = _AudioDeviceManager->GetAudioCaptureList();
    if (pList->GetCount() <= 0) return "";

    _audioCaptureDeviceArray.clear();
    Json::Value arrayObj;
    for (size_t i = 0; i < pList->GetCount(); i++)
    {
      Json::Value deviceInfo;
      AliEngineDeviceInfo device = pList->GetDeviceInfo(i);
      deviceInfo["deviceId"] = device.deviceID.c_str();
      deviceInfo["deviceName"] = device.deviceName.c_str();
      arrayObj.append(deviceInfo);
    }
    pList->Release();
    _audioCaptureDeviceArray = arrayObj.toStyledString();
    return _audioCaptureDeviceArray.c_str();
  }
  return "";
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetCurrentAudioCaptureName()
{
  if (_AudioDeviceManager)
  {
    _audioCaptureNameId = _AudioDeviceManager->GetCurrentAudioCaptureName().c_str();
    return _audioCaptureNameId.c_str();
  }

  return "";
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetCurrentAudioCaptureId()
{
  if (_AudioDeviceManager)
  {
    _audioCaptureNameId = _AudioDeviceManager->GetCurrentAudioCaptureID().c_str();
    return _audioCaptureNameId.c_str();
  }

  return "";
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetCurrentAudioCaptureByName(const char* deviceName)
{
  if (_AudioDeviceManager && deviceName != nullptr)
  {
    return _AudioDeviceManager->SetCurrentAudioCaptureName(deviceName);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetCurrentAudioCaptureById(const char* deviceId)
{
  if (_AudioDeviceManager && deviceId != nullptr)
  {
    return _AudioDeviceManager->SetCurrentAudioCaptureID(deviceId);
  }
  return -1;
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetAudioRenderers()
{
  if (_AudioDeviceManager)
  {
    AliEngineDeviceInfoList* pList = _AudioDeviceManager->GetAudioPlayerList();
    if (pList->GetCount() <= 0) return "";

    _audioRenderDeviceArray.clear();
    Json::Value arrayObj;
    for (size_t i = 0; i < pList->GetCount(); i++)
    {
      Json::Value deviceInfo;
      AliEngineDeviceInfo device = pList->GetDeviceInfo(i);
      deviceInfo["deviceId"] = device.deviceID.c_str();
      deviceInfo["deviceName"] = device.deviceName.c_str();
      arrayObj.append(deviceInfo);
    }

    pList->Release();
    _audioRenderDeviceArray = arrayObj.toStyledString();
    return _audioRenderDeviceArray.c_str();
  }
  return "";
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetCurrentAudioRendererName()
{
  if (_AudioDeviceManager)
  {
    _audioRenderNameId = _AudioDeviceManager->GetCurrentAudioPlayerName().c_str();
    return _audioRenderNameId.c_str();
  }

  return "";
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetCurrentAudioRendererId()
{
  if (_AudioDeviceManager)
  {
    _audioRenderNameId = _AudioDeviceManager->GetCurrentAudioPlayerID().c_str();
    return _audioRenderNameId.c_str();
  }

  return "";
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetCurrentAudioRendererByName(const char* deviceName)
{
  if (_AudioDeviceManager && deviceName != nullptr)
  {
    return _AudioDeviceManager->SetCurrentAudioPlayerName(deviceName);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetCurrentAudioRendererById(const char* deviceId)
{
  if (_AudioDeviceManager && deviceId != nullptr)
  {
    return _AudioDeviceManager->SetCurrentAudioPlayerID(deviceId);
  }
  return -1;
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetCameraList()
{
  if (_VideoDeviceManager)
  {
    AliEngineDeviceInfoList* pList = _VideoDeviceManager->GetCameraList();
    if (pList->GetCount() <= 0) return "";

    _cameraDeviceArray.clear();
    Json::Value arrayObj;
    for (size_t i = 0; i < pList->GetCount(); i++)
    {
      Json::Value deviceInfo;
      AliEngineDeviceInfo device = pList->GetDeviceInfo(i);
      deviceInfo["deviceId"] = device.deviceID.c_str();
      deviceInfo["deviceName"] = device.deviceName.c_str();
      arrayObj.append(deviceInfo);
    }

    pList->Release();
    _cameraDeviceArray = arrayObj.toStyledString();
    return _cameraDeviceArray.c_str();
  }
  return "";
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetCurrentCameraName()
{
  if (_VideoDeviceManager)
  {
    _cameraNameId = _VideoDeviceManager->GetCurrentCameraName().c_str();
    return _cameraNameId.c_str();
  }

  return "";
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetCurrentCameraId()
{
  if (_VideoDeviceManager)
  {
    _cameraNameId = _VideoDeviceManager->GetCurrentCameraID().c_str();
    return _cameraNameId.c_str();
  }

  return "";
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetCurrentCameraByName(const char* deviceName)
{
  if (_VideoDeviceManager && deviceName != nullptr)
  {
    return _VideoDeviceManager->SetCurrentCameraName(deviceName);
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetCurrentCameraById(const char* deviceId)
{
  if (_VideoDeviceManager && deviceId != nullptr)
  {
    return _VideoDeviceManager->SetCurrentCameraID(deviceId);
  }

  return -1;
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsUserOnline(const char* userId, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->IsUserOnline(userId);
  }
  return false;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcStartAudioCapture()
{
  if (_sharedEngine)
  {
    _sharedEngine->StartAudioCapture();
  }
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcStopAudioCapture()
{
  if (_sharedEngine)
  {
    _sharedEngine->StopAudioCapture();
  }
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcStartAudioPlayer()
{
  if (_sharedEngine)
  {
    _sharedEngine->StartAudioPlayer();
  }
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcStopAudioPlayer()
{
  if (_sharedEngine)
  {
    _sharedEngine->StopAudioPlayer();
  }
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetLogLevel(AliRTClogLevel logLevel)
{
  if (logLevel < 0 || logLevel > AliEngineLogLevelNone)
  {
    return;
  }

  if (_sharedEngine)
  {
    _sharedEngine->SetLogLevel((AliEngineLogLevel)logLevel);
  }
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetLogDirPath(const char* logDirPath)
{
  return AliEngine::SetLogDirPath(logDirPath);
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcUploadLog()
{
  AliEngine::UploadLog();
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsCameraOn(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->IsCameraOn();
  }
  return false;
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsInCall(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->IsInCall();
  }
  return false;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetH5CompatibleMode(bool comp)
{
  AliEngine::SetH5CompatibleMode(comp);
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcGetH5CompatibleMode()
{
  return AliEngine::GetH5CompatibleMode();
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetChannelProfile(AliRTCChannelProfile profile, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->SetChannelProfile((AliEngineChannelProfile)profile);
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetClientRole(AliRTCClientRole role, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->SetClientRole((AliEngineClientRole)role);
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetRecordingVolume(int volume, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->SetRecordingVolume(volume);
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetPlayoutVolume(int volume, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->SetPlayoutVolume(volume);
  }

  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcEnableAudioVolumeIndication(int interval, int smooth, int reportVad, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->EnableAudioVolumeIndication(interval, smooth, reportVad);
  }

  return -1;
}

ALIRTCSDK_API int  AliRtcSetBeautyEffect(bool enable, const float whiteningLevel, const float smoothnessLevel)
{
  if (_sharedEngine)
  {
    AliEngineBeautyConfig config;
    config.smoothnessLevel = smoothnessLevel;
    config.whiteningLevel = whiteningLevel;
    return _sharedEngine->SetBeautyEffect(enable, config);
  }

  return -1;
}

ALIRTCSDK_API int  AliRtcAddVideoWatermark(AliRTCVideoTrack track, const char* imageUrl, bool visibleInPreview,
  const float Portrait_x, const float Portrait_y, const float Portrait_w, const float Portrait_h,
  const float Landscape_x, const float Landscape_y, const float Landscape_w, const float Landscape_h,
  const float alpha, bool normalized)
{
  if (nullptr == imageUrl)
    return -1;

  if (_sharedEngine)
  {
    AliEngineWaterMarkConfig config;
    config.alpha = alpha;
    config.visibleInPreview = visibleInPreview;
    config.normalized = normalized;

    config.positionInPortraitMode.x = Portrait_x;
    config.positionInPortraitMode.y = Portrait_y;
    config.positionInPortraitMode.w = Portrait_w;
    config.positionInPortraitMode.h = Portrait_h;

    config.positionInPortraitMode.x = Landscape_x;
    config.positionInPortraitMode.y = Landscape_y;
    config.positionInPortraitMode.w = Landscape_w;
    config.positionInPortraitMode.h = Landscape_h;

    return _sharedEngine->AddVideoWatermark((AliEngineVideoTrack)track, imageUrl, config);
  }

  return -1;
}

ALIRTCSDK_API int AliRtcAddRecordTemplate(AliRTCRecordType recordType,
    AliRTCRecordFormat recordFormat,
    AliRTCAudioSampleRate sampleRate,
    AliRTCAudioQuality audioQuality,
    bool externalPcmCaptureRecording,
    bool externalPcmRenderRecording,
    int canvasWidth,
    int canvasHeight,
    bool isFragment,
    int fps,
    int bitrate,
    AliRTCRecordVideoLayoutMode mode,
    unsigned char r,
    unsigned char g,
    unsigned char b)
{
    if (_sharedEngine)
    {
        _RecordVideoLayout.mode = (AliEngineRecordVideoLayoutMode)mode;
        _RecordVideoLayout.backColor.b = b;
        _RecordVideoLayout.backColor.g = g;
        _RecordVideoLayout.backColor.r = r;

        _RecordTemplate.audioQuality = (AliEngineAudioQuality)audioQuality;
        _RecordTemplate.bitrate = bitrate;
        _RecordTemplate.canvas.canvasHeight = canvasHeight;
        _RecordTemplate.canvas.canvasWidth = canvasWidth;
        _RecordTemplate.externalPcmCaptureRecording = externalPcmCaptureRecording;
        _RecordTemplate.externalPcmRenderRecording = externalPcmRenderRecording;
        _RecordTemplate.fps = fps;
        _RecordTemplate.isFragment = isFragment;
        _RecordTemplate.recordFormat = (AliEngineRecordFormat)recordFormat;
        _RecordTemplate.recordType = (AliEngineRecordType)recordType;
        _RecordTemplate.sampleRate = (AliEngineAudioSampleRate)sampleRate;
        return _sharedEngine->AddRecordTemplate(_RecordTemplate);
    }
    return -1;
}

ALIRTCSDK_API int AliRtcAddRecordUserStream(const char* uid)
{
  if (uid)
  {
    mMutexRecordUserStreams.lock();
    mMapRecordUserStreams[uid] = true;
    mMutexRecordUserStreams.unlock();
    return 0;
  }
  return -1;
}

ALIRTCSDK_API int AliRtcRemoveRecordUserStream(const char* uid)
{
  if (uid)
  {
    mMutexRecordUserStreams.lock();
    mMapRecordUserStreams[uid] = false;
    mMapRecordUserStreams.erase(uid);
    mMutexRecordUserStreams.unlock();    
    return 0;
  }
  return -1;
}

ALIRTCSDK_API int AliRtcClearAllRecordUserStreams()
{
  mMutexRecordUserStreams.lock();
  for (auto iter : mMapRecordUserStreams)
  {
    iter.second = false;
  }
  mMapRecordUserStreams.clear();
  mMutexRecordUserStreams.unlock();
  return 0;
}

ALIRTCSDK_API bool AliRtcUpdateRecordUserStreams(const char* uid /*= ""*/)
{
  if (nullptr == uid) return false;
  std::string strUid = uid;
  if (!strUid.empty() && mMapRecordUserStreams.find(uid) == mMapRecordUserStreams.end())
  {
    return false;
  }

  if (_sharedEngine && _sharedEngine->IsInCall())
  {
    /*更新订阅内容列表 */
    _RecordVideoLayout.shapes.Clear();

    if (_sharedEngine->IsScreenSharePublished())
    {
      AliEngineRecordVideoRegion region_screen;
      region_screen.userId = "";
      region_screen.sourceType = AliEngineVideoSourceScreenShare;
      _RecordVideoLayout.shapes.AddRecordVideoRegion(region_screen);
    }

    if (_sharedEngine->IsLocalVideoStreamPublished())
    {
      AliEngineRecordVideoRegion region;
      region.userId = "";
      region.sourceType = AliEngineVideoSourceCamera;
      _RecordVideoLayout.shapes.AddRecordVideoRegion(region);
    }

    for (auto iter : mMapRecordUserStreams)
    {
      std::string uid = iter.first;
      Dictionary dict;
      if (iter.second == false || _sharedEngine->GetUserInfo(uid.c_str(), dict) != 0)
        continue;

      bool hasScreenStream = dict.getBoolValue("hasScreenSharing", false);
      bool hasCameraMaster = dict.getBoolValue("hasCameraMaster", false);
      bool hasCameraSlave = dict.getBoolValue("hasCameraSlave", false);
      bool hasCamera = hasCameraMaster | hasCameraSlave;

      if (hasScreenStream)
      {
        AliEngineRecordVideoRegion region_screen;
        region_screen.userId = _strdup(uid.c_str());
        region_screen.sourceType = AliEngineVideoSourceScreenShare;
        _RecordVideoLayout.shapes.AddRecordVideoRegion(region_screen);
      }

      if (hasCamera)
      {
        AliEngineRecordVideoRegion region;
        region.userId = _strdup(uid.c_str());
        region.sourceType = AliEngineVideoSourceCamera;
        _RecordVideoLayout.shapes.AddRecordVideoRegion(region);
      }
    }

    return _sharedEngine->UpdateRecordLayout(_RecordVideoLayout);
  }
  return false;
}

ALIRTCSDK_API int AliRtcStartRecord(const char* filePath)
{
  if (_sharedEngine)
  {
    std::string strFormat, strSaveFile = filePath;
    switch (_RecordTemplate.recordType)
    {
    case AliEngineRecordType::AliEngineRecordTypeAudio:
    {
      if (_RecordTemplate.recordFormat == AliEngineRecordFormat::AliEngineRecordFormatAAC)
        strFormat = ".aac";
      else if (_RecordTemplate.recordFormat == AliEngineRecordFormat::AliEngineRecordFormatWAV)
        strFormat = ".wav";

      strSaveFile += "/RecordTypeAudio_";
      break;
    }
    case AliEngineRecordType::AliEngineRecordTypeVideo:
    {
      strSaveFile += "/RecordTypeVideo_";
      strFormat = ".mp4";
      break;
    }
    case AliEngineRecordType::AliEngineRecordTypeBoth:
    {
      strSaveFile += "/RecordTypeBoth_";
      strFormat = ".mp4";
      break;
    }
    }

    time_t myt = time(NULL);
    strSaveFile += std::to_string(myt);
    strSaveFile += strFormat;

    return _sharedEngine->StartRecord(strSaveFile.c_str(), _RecordVideoLayout);
  }
  return -1;
}

ALIRTCSDK_API bool AliRtcPauseRecord()
{
  if (_sharedEngine)
  {
    return _sharedEngine->PauseRecord();
  }
  return false;
}

ALIRTCSDK_API bool AliRtcResumeRecord()
{
  if (_sharedEngine)
  {
    return _sharedEngine->ResumeRecord();
  }
  return false;
}

ALIRTCSDK_API bool AliRtcStopRecord()
{
  if (_sharedEngine)
  {
    AliRtcClearAllRecordUserStreams();
    return _sharedEngine->StopRecord();
  }
  return false;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSubscribeAudioData(AliRTCAudioSource audioSource, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    pMediaEngine->SubscribeAudioData((AliEngineAudioSource)audioSource);
    return 0;
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcUnsubscribeAudioData(AliRTCAudioSource audioSource, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    pMediaEngine->UnsubscribeAudioData((AliEngineAudioSource)audioSource);
    return 0;
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetSubscribeAudioNumChannel(int audioNumChannel, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    pMediaEngine->SetSubscribeAudioNumChannel((AliEngineAudioNumChannelType)audioNumChannel);
    return 0;
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetSubscribeAudioSampleRate(int audioSampleRate, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    pMediaEngine->SetSubscribeAudioSampleRate((AliEngineAudioSampleRate)audioSampleRate);
    return 0;
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcRegisterAudioFrameObserver(const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    auto iter = _mapAudioFrameObserver.find(channel);
    if (iter != _mapAudioFrameObserver.end())
      return 0;
    else
    {
      IAudioFrameObserver* pObserver = new AliRtcAudioFrameObserver(String(channel));
      if (pObserver)
      {
        pMediaEngine->RegisterAudioFrameObserver(pObserver);
        _mapAudioFrameObserver[channel] = pObserver;
        return 0;
      }
    }
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcUnRegisterAudioFrameObserver(const char *channel)
{
    IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
    if (pMediaEngine)
    {
        auto iter = _mapAudioFrameObserver.find(channel);
        if (iter != _mapAudioFrameObserver.end())
        {
            if (iter->second)
            {
                pMediaEngine->UnRegisterAudioFrameObserver(iter->second);
                delete iter->second;
            }
            _mapAudioFrameObserver.erase(channel);
        }
        return 0;
    }
    return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcRegisterVideoSampleObserver(const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    auto iter = _mapVideoFrameObserver.find(channel);
    if (iter != _mapVideoFrameObserver.end())
      return 0;
    else
    {
      IVideoFrameObserver* pObserver = new AliRtcVideoFrameObserver(String(channel));
      if (pObserver)
      {
        pMediaEngine->RegisterVideoSampleObserver(pObserver);
        _mapVideoFrameObserver[channel] = pObserver;
        return 0;
      }
    }
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcUnRegisterVideoSampleObserver(const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    auto iter = _mapVideoFrameObserver.find(channel);
    if (iter != _mapVideoFrameObserver.end())
    {
      if (iter->second)
      {
        pMediaEngine->UnRegisterVideoSampleObserver(iter->second);
        delete iter->second;
      }
      _mapVideoFrameObserver.erase(channel);
    }
    return 0;
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetExternalVideoSource(bool enable, bool useTexture, AliRTCVideoSource sourceType, int renderMode, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->SetExternalVideoSource(enable, useTexture, (AliEngineVideoTrack)sourceType, (AliEngineRenderMode)renderMode);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcPushExternalVideoFrame(AliRTCVideoDataSample& frame, AliRTCVideoSource sourceType, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine && frame.dataPtr && frame.dataLength > 0)
  {
    AliEngineVideoRawData videoSample;
    videoSample.dataPtr = frame.dataPtr;
    videoSample.dataLength = frame.dataLength;
    videoSample.format = (AliEngineVideoFormat)frame.format;
    videoSample.type = (AliEngineBufferType)frame.bufferType;
    videoSample.height = frame.height;
    videoSample.width = frame.width;
    videoSample.rotation = frame.rotation;
    videoSample.strideY = frame.strideY;
    videoSample.strideU = frame.strideU;
    videoSample.strideV = frame.strideV;
    videoSample.timeStamp = frame.timeStamp;
    return pMediaEngine->PushExternalVideoFrame(videoSample, (AliEngineVideoTrack)sourceType);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetExternalAudioSource(bool enable, unsigned int sampleRate, unsigned int channelsPerFrame, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->SetExternalAudioSource(enable, sampleRate, channelsPerFrame);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcPushExternalAudioFrameRawData(const void* audioSamples, unsigned int sampleLength, long long timestamp, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->PushExternalAudioFrameRawData(audioSamples, sampleLength, timestamp);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetExternalAudioPublishVolume(int volume, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->SetExternalAudioPublishVolume(volume);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcGetExternalAudioPublishVolume(const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->GetExternalAudioPublishVolume();
  }
  return 0;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetMixedWithMic(bool mixed, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->SetMixedWithMic(mixed);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetExteranlAudioRender(bool enable, unsigned int sampleRate, unsigned int channelsPerFrame, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->SetExteranlAudioRender(enable, sampleRate, channelsPerFrame);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcPushExternalAudioRenderRawData(const void* audioSamples, unsigned int sampleLength, unsigned int sampleRate, unsigned int channelsPerFrame, long long timestamp, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->PushExternalAudioRenderRawData(audioSamples, sampleLength, sampleRate, channelsPerFrame, timestamp);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetExternalAudioRenderVolume(int volume, const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->SetExternalAudioRenderVolume(volume);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcGetExternalAudioRenderVolume(const char *channel)
{
  IAliEngineMediaEngine* pMediaEngine = GetMediaEngine(channel);
  if (pMediaEngine)
  {
    return pMediaEngine->GetExternalAudioRenderVolume();
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStartTestAudioRecordById(const char* deviceId, int callbackInterval/* = 0*/)
{
  if (_AudioDeviceManager && deviceId != nullptr)
  {
    return _AudioDeviceManager->StartTestAudioRecordById(deviceId, callbackInterval);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStartTestAudioRecordByName(const char* deviceName, int callbackInterval/* = 0*/)
{
  if (_AudioDeviceManager && deviceName != nullptr)
  {
    return _AudioDeviceManager->StartTestAudioRecordByName(deviceName, callbackInterval);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStopTestAudioRecord()
{
  if (_AudioDeviceManager)
  {
    return _AudioDeviceManager->StopTestAudioRecord();
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStartTestAudioPlayoutById(const char* deviceId, const char *wavFilePath, int callbackInterval /*= 0*/, int loopCycles/* = 0*/)
{
  if (_AudioDeviceManager && deviceId != nullptr)
  {
    return _AudioDeviceManager->StartTestAudioPlayoutById(deviceId, wavFilePath, callbackInterval, loopCycles);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStartTestAudioPlayoutByName(const char* deviceName, const char *wavFilePath, int callbackInterval/* = 0*/, int loopCycles/* = 0*/)
{
  if (_AudioDeviceManager && deviceName != nullptr)
  {
    return _AudioDeviceManager->StartTestAudioPlayoutByName(deviceName, wavFilePath, callbackInterval, loopCycles);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStopTestAudioPlayout()
{
  if (_AudioDeviceManager)
  {
    return _AudioDeviceManager->StopTestAudioPlayout();
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetLocalViewConfig(void *displayView, int backgroundColor, int renderMode, int mirrorMode, int scaleMode, int rotation, long long renderId, int width, int height, bool enableGL, AliRTCVideoTrack track)
{
  if (_sharedEngine)
  {
    AliEngineVideoCanvas canv;
    canv.backgroundColor = backgroundColor;
    canv.displayView = displayView;
    canv.enableGL = enableGL;
    canv.height = height;
    canv.mirrorMode = (AliEngineRenderMirrorMode)mirrorMode;
    canv.renderId = renderId;
    canv.renderMode = (AliEngineRenderMode)renderMode;
    canv.rotation = (AliEngineRotationMode)rotation;
    canv.scaleMode = (AliEngineVideoScale)scaleMode;
    canv.width = width;
    return _sharedEngine->SetLocalViewConfig(canv, (AliEngineVideoTrack)track);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcSetRemoteViewConfig(void *displayView, int backgroundColor, int renderMode, int mirrorMode, int scaleMode, int rotation, long long renderId, int width, int height, bool enableGL, const char* userId, AliRTCVideoTrack track, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    AliEngineVideoCanvas canv;
    canv.backgroundColor = backgroundColor;
    canv.displayView = displayView;
    canv.enableGL = enableGL;
    canv.height = height;
    canv.mirrorMode = (AliEngineRenderMirrorMode)mirrorMode;
    canv.renderId = renderId;
    canv.renderMode = (AliEngineRenderMode)renderMode;
    canv.rotation = (AliEngineRotationMode)rotation;
    canv.scaleMode = (AliEngineVideoScale)scaleMode;
    canv.width = width;
    return _sharedEngine->SetRemoteViewConfig(canv, userId, (AliEngineVideoTrack)track);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStartScreenShareByDesktopId(unsigned int desktopId, bool isShareByRegion, float originX, float originY, float width, float height, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    AliEngineScreenShareConfig share;
    share.isShareByRegion = isShareByRegion;
    share.shareRegion.height = height;
    share.shareRegion.originX = originX;
    share.shareRegion.originY = originY;
    share.shareRegion.width = width;
    pEngine->StartScreenShareByDesktopId(desktopId, share);
  }
  return -1;
}

ALIRTCSDK_API int ALIRTCSDK_CALL AliRtcStopScreenShare(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    return pEngine->StopScreenShare();
  }
  return -1;
}

ALIRTCSDK_API const char* ALIRTCSDK_CALL AliRtcGetScreenShareSourceInfo(AliRTCScreenShareType sourceType, const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
  {
    AliEngineScreenSourceList* pList = pEngine->GetScreenShareSourceInfo((AliEngineScreenShareType)sourceType);
    if (pList->GetCount() <= 0) return "";

    _screenSourceArray.clear();
    Json::Value arrayObj;
    for (size_t i = 0; i < pList->GetCount(); i++)
    {
      Json::Value deviceInfo;
      AliEngineScreenSourcInfo source = pList->GetSourceInfo(i);
      deviceInfo["sourceId"] = source.sourceId.c_str();
      deviceInfo["sourceName"] = source.sourceName.c_str();
      arrayObj.append(deviceInfo);
    }

    pList->Release();
    _screenSourceArray = arrayObj.toStyledString();
    return _screenSourceArray.c_str();
  }
  return "";
}

ALIRTCSDK_API bool ALIRTCSDK_CALL AliRtcIsScreenSharePublished(const char *channel)
{
  AliEngine* pEngine = GetEngine(channel);
  if (pEngine)
    return pEngine->IsScreenSharePublished();
  return false;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnJoinChannelNotify(ALIRTC_FUNC_OnJoinChannelNotify onJoinChannelNotify)
{
  _onJoinChannelNotify = onJoinChannelNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnAudioPublishStateChangedNotify(ALIRTC_FUNC_OnAudioPublishStateChangedNotify onAudioPublishStateChangedNotify)
{
  _onAudioPublishStateChangedNotify = onAudioPublishStateChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnVideoPublishStateChangedNotify(ALIRTC_FUNC_OnVideoPublishStateChangedNotify onVideoPublishStateChangedNotify)
{
  _onVideoPublishStateChangedNotify = onVideoPublishStateChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnDualStreamPublishStateChangedNotify(ALIRTC_FUNC_OnDualStreamPublishStateChangedNotify onDualStreamPublishStateChangedNotify)
{
  _onDualStreamPublishStateChangedNotify = onDualStreamPublishStateChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnScreenSharePublishStateChangedNotify(ALIRTC_FUNC_OnScreenSharePublishStateChangedNotify onScreenSharePublishStateChangedNotify)
{
  _onScreenSharePublishStateChangedNotify = onScreenSharePublishStateChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnAudioSubscribeStateChangedNotify(ALIRTC_FUNC_OnAudioSubscribeStateChangedNotify onAudioSubscribeStateChangedNotify)
{
  _onAudioSubscribeStateChangedNotify = onAudioSubscribeStateChangedNotify;
}
ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnVideoSubscribeStateChangedNotify(ALIRTC_FUNC_OnVideoSubscribeStateChangedNotify onVideoSubscribeStateChangedNotify)
{
  _onVideoSubscribeStateChangedNotify = onVideoSubscribeStateChangedNotify;
}
ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnScreenShareSubscribeStateChangedNotify(ALIRTC_FUNC_OnScreenShareSubscribeStateChangedNotify onScreenShareSubscribeStateChangedNotify)
{
  _onScreenShareSubscribeStateChangedNotify = onScreenShareSubscribeStateChangedNotify;
}
ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnSubscribeStreamTypeChangedNotify(ALIRTC_FUNC_OnSubscribeStreamTypeChangedNotify onSubscribeStreamTypeChangedNotify)
{
  _onSubscribeStreamTypeChangedNotify = onSubscribeStreamTypeChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnRemoteUserOnLineNotify(ALIRTC_FUNC_OnRemoteUserOnLineNotify onRemoteUserOnLineNotify)
{
  _onRemoteUserOnLineNotify = onRemoteUserOnLineNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnRemoteUserOffLineNotify(ALIRTC_FUNC_OnRemoteUserOffLineNotify onRemoteUserOffLineNotify)
{
  _onRemoteUserOffLineNotify = onRemoteUserOffLineNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnRemoteTrackAvailableNotify(ALIRTC_FUNC_OnRemoteTrackAvailableNotify onRemoteTrackAvailableNotify)
{
  _onRemoteTrackAvailableNotify = onRemoteTrackAvailableNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnNetworkQualityChangedNotify(ALIRTC_FUNC_OnNetworkQualityChangedNotify onNetworkQualityChangedNotify)
{
  _onNetworkQualityChangedNotify = onNetworkQualityChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnByeNotify(ALIRTC_FUNC_OnByeNotify onByeNotify)
{
  _onByeNotify = onByeNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnOccurWarningNotify(ALIRTC_FUNC_OnOccurWarningNotify onOccurWarningNotify)
{
  _onOccurWarningNotify = onOccurWarningNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnOccurErrorNotify(ALIRTC_FUNC_OnOccurErrorNotify onOccurErrorNotify)
{
  _onOccurErrorNotify = onOccurErrorNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnConnectionLostNotify(ALIRTC_FUNC_OnConnectionLostNotify onConnectionLostNotify)
{
  _onConnectionLostNotify = onConnectionLostNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnTryToReconnectNotify(ALIRTC_FUNC_OnTryToReconnectNotify onTryToReconnectNotify)
{
  _onTryToReconnectNotify = onTryToReconnectNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnConnectionRecoveryNotify(ALIRTC_FUNC_OnConnectionRecoveryNotify onConnectionRecoveryNotify)
{
  _onConnectionRecoveryNotify = onConnectionRecoveryNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnLeaveChannelResultNotify(ALIRTC_FUNC_OnLeaveChannelResultNotify onLeaveChannelResultNotify)
{
  _onLeaveChannelResultNotify = onLeaveChannelResultNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnUpdateRoleNotify(ALIRTC_FUNC_OnUpdateRoleNotify onUpdateRoleNotify)
{
  _onUpdateRoleNotify = onUpdateRoleNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnAudioDeviceStateChangedNotify(ALIRTC_FUNC_OnAudioDeviceStateChangedNotify onAudioDeviceStateChangedNotify)
{
  _onAudioDeviceStateChangedNotify = onAudioDeviceStateChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnVideoDeviceStateChangedNotify(ALIRTC_FUNC_OnVideoDeviceStateChangedNotify onVideoDeviceStateChangedNotify)
{
  _onVideoDeviceStateChangedNotify = onVideoDeviceStateChangedNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnAudioVolumeCallbackNotify(ALIRTC_FUNC_OnAudioVolumeCallback onAudioVolumeCallbackNotify)
{
  _onAudioVolumeCallbackNotify = onAudioVolumeCallbackNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnAudioDeviceRecordLevelNotify(ALIRTC_FUNC_OnAudioDeviceRecordLevel onAudioDeviceRecordLevelNotify)
{
  _onAudioDeviceRecordLevelNotify = onAudioDeviceRecordLevelNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnAudioDevicePlayoutLevelNotify(ALIRTC_FUNC_OnAudioDevicePlayoutLevel onAudioDevicePlayoutLevelNotify)
{
  _onAudioDevicePlayoutLevelNotify = onAudioDevicePlayoutLevelNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnAudioDevicePlayoutEndNotify(ALIRTC_FUNC_OnAudioDevicePlayoutEnd onAudioDevicePlayoutEndNotify)
{
  _onAudioDevicePlayoutEndNotify = onAudioDevicePlayoutEndNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnLastmileDetectResultWithQualityNotify(ALIRTC_FUNC_OnLastmileDetectResultWithQuality onLastmileDetectResultWithQualityNotify)
{
  _onLastmileDetectResultWithQualityNotify = onLastmileDetectResultWithQualityNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnMediaRecordEventNotify(ALIRTC_FUNC_OnMediaRecordEvent onMediaRecordEventNotify)
{
  _onMediaRecordEventNotify = onMediaRecordEventNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnRawDataAudioFrameNotify(ALIRTC_FUNC_OnRawDataAudioFrame onRawDataAudioFrameNotify)
{
  _onRawDataAudioFrameNotify = onRawDataAudioFrameNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnPublishAudioFrameNotify(ALIRTC_FUNC_OnPublishAudioFrame onPublishAudioFrameNotify)
{
  _onPublishAudioFrameNotify = onPublishAudioFrameNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnSubscribeAudioFrameNotify(ALIRTC_FUNC_OnSubscribeAudioFrame onSubscribeAudioFrameNotify)
{
  _onSubscribeAudioFrameNotify = onSubscribeAudioFrameNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnPlaybackAudioFrameBeforeMixingNotify(ALIRTC_FUNC_OnPlaybackAudioFrameBeforeMixing onPlaybackAudioFrameBeforeMixingNotify)
{
  _onPlaybackAudioFrameBeforeMixingNotify = onPlaybackAudioFrameBeforeMixingNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnCaptureVideoSampleNotify(ALIRTC_FUNC_OnCaptureVideoSample onCaptureVideoSampleNotify)
{
  _onCaptureVideoSampleNotify = onCaptureVideoSampleNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnPreEncodeVideoSampleNotify(ALIRTC_FUNC_OnPreEncodeVideoSample onPreEncodeVideoSampleNotify)
{
  _onPreEncodeVideoSampleNotify = onPreEncodeVideoSampleNotify;
}

ALIRTCSDK_API void ALIRTCSDK_CALL AliRtcSetEventOnRemoteVideoVideoSampleNotify(ALIRTC_FUNC_OnRemoteVideoVideoSample onRemoteVideoVideoSampleNotify)
{
  _onRemoteVideoVideoSampleNotify = onRemoteVideoVideoSampleNotify;
}

