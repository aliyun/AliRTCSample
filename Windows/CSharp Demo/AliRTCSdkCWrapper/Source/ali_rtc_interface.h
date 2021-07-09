#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <list>
#if defined(ALISDK_STATIC)
#define ALI_SDK_API
#else
#if defined(ALISDK_EXPORTS)
#define ALI_SDK_API __declspec(dllexport)
#else
#define ALI_SDK_API __declspec(dllimport)
#endif
#endif

#define MAX_MEDIA_STREAM_CHARS  (64)
#define MAX_MEDIA_TRACK_CHARS   (64)
#define MAX_VIDEO_TRACKS        (8)


extern "C" void set_h5_compatible_mode(int enable);
extern "C" int is_h5_compatible_mode();

extern "C" void set_software_3a_states(bool aec_enable, bool ans_enable, bool agc_enable);

namespace AliRTCSdk_Windows {
enum AliRTCSDK_Channel_Profile {
  AliRTCSDK_Communication,
  AliRTCSDK_Interactive_live,
  AliRTCSDK_CDN_live,
};

enum AliRTCSDK_Client_Role {
  AliRTCSDK_Interactive,
  AliRTCSDK_live,
};

enum AliRTCSdk_VideoSource_Type {
  AliRTCSDK_VideoSource_Type_CameraLarge = 0,
  AliRTCSDK_VideoSource_Type_CameraSmall,
  AliRTCSDK_VideoSource_Type_ScreenShare,
  AliRTCSDK_VideoSource_Type_Super,
  AliRTCSDK_VideoSource_Type_MAX,
};

enum ALI_SDK_API AliRTCSDK_Warning_Event {
  AliRTCSDK_Warning_Event_NoWarning = 0,
  AliRTCSDK_Warning_Event_Reconnecting = 1,
  AliRTCSDK_Warning_Event_Poor_Network = 0x0101,
  AliRTCSDK_Warning_Event_Recover_Network = 0x0102,
};

enum ALI_SDK_API AliRTCSDK_Error_Event {
  AliRTCSDK_Error_Event_NoError = 0,
  AliRTCSDK_Error_Event_Video_Device = 1,                 //视频设备错误
  AliRTCSDK_Error_Event_Audio_Device = 2,                 //音频设备错误
  AliRTCSDK_Error_Event_Device = 3,                       //设备错误
  AliRTCSDK_Error_Event_Server = 4,                       //服务器错误
  AliRTCSDK_Error_Event_Network_Timeout = 5,              //网络超时
  AliRTCSDK_Error_Event_Polling_Error = 6,                //会话断开
  AliRTCSDK_Error_Event_HeartBeat_Timeout = 7,            //心跳超时
  AliRTCSDK_Error_Event_Parameter_Invaild = 8,            //参数错误
  AliRTCSDK_Error_Event_State_Error = 9,                  //状态错误
  AliRTCSDK_Error_Event_ICE_Connect_Timeout = 10,         //连接超时失败
  AliRTCSDK_Error_Event_ICE_Connect_Failed_Timeout = 11,  //连接断开重试超时失败
  ///////// joinChannel相关 ////////////
  AliRTCSDK_Error_Event_Join_BadAppID = 13,                      // AppID不存在
  AliRTCSDK_Error_Event_Join_InvalidAppID = 14,                  // AppID已失效
  AliRTCSDK_Error_Event_Join_InvalidChannel = 15,                // channel已过期
  AliRTCSDK_Error_Event_Join_BadChannel = 16,                    // channel不存在
  AliRTCSDK_Error_Event_Join_BadToken = 17,                      // 加入频道失败，token无效
  AliRTCSDK_Error_Event_Join_Timeout = 18,                       // 加入频道超时
  AliRTCSDK_Error_Event_Join_BadParam = 19,                      // 参数异常，存在空字段
  ///////// 设备相关  ////////////
  AliRTCSDK_Error_Event_Mic_OpenFail = 20,                       // 麦克风打开失败
  AliRTCSDK_Error_Event_Speaker_OpenFail = 21,                   // 扬声器打开失败
  AliRTCSDK_Error_Event_Camera_OpenFail = 22,                    // 摄像头打开失败
  AliRTCSDK_Error_Event_Mic_Interrupt = 23,                      // 麦克风采集被中断
  AliRTCSDK_Error_Event_Camera_Interrupt = 24,                   // 摄像头采集被中断
  ///////// SDK相关 ////////////
  AliRTCSDK_Error_Event_NoEngineInstance = 25,                   // SDK实例不存在或者已经被销毁
  AliRTCSDK_Error_Event_SDKInvalidState = 26,                    // 状态错误
  AliRTCSDK_Error_Event_Low_Performance = 27,                    // SDK性能不足
  AliRTCSDK_Error_Event_Restore_Performace = 28,                 // SDK性能恢复

  AliRTCSDK_Error_Event_Unknown_Error = 999,                     // 未知错误
};


enum ALI_SDK_API AliRTCSDK_Render_Mode {
  AliRTCSDK_Render_Mode_Fit = 0,
};

enum ALI_SDK_API Ali_RTC_Client_Mode {
  Ali_RTC_Client_Mode_SendOnly = 0,
  Ali_RTC_Client_Mode_ReceiveOnly,
  Ali_RTC_Client_Mode_SendReceive,
  Ali_RTC_Client_Mode_Auido_SendReceive,
};

enum ALI_SDK_API AliRTCSDK_Capture_Type {
  AliRTCSDK_Capture_Typ_Invalid = 0,
  AliRTCSDK_Capture_Typ_Front = 0,
  AliRTCSDK_Capture_Typ_Back = 1,
};

enum AliRTCSDK_Video_Profile {
  AliRTCSDK_Video_Profile_Default = 0,
  AliRTCSDK_Video_Profile_180_240P_15,
  AliRTCSDK_Video_Profile_180_320P_15,
  AliRTCSDK_Video_Profile_180_320P_30,
  AliRTCSDK_Video_Profile_240_320P_15,
  AliRTCSDK_Video_Profile_360_480P_15,
  AliRTCSDK_Video_Profile_360_480P_30,
  AliRTCSDK_Video_Profile_360_640P_15,
  AliRTCSDK_Video_Profile_360_640P_30,
  AliRTCSDK_Video_Profile_480_640P_15,
  AliRTCSDK_Video_Profile_480_640P_30,
  AliRTCSDK_Video_Profile_720_960P_15,
  AliRTCSDK_Video_Profile_720_960P_30,
  AliRTCSDK_Video_Profile_720_1280P_15,
  AliRTCSDK_Video_Profile_720_1280P_30,
  AliRTCSDK_Video_Profile_1080_1920P_15,
  AliRTCSDK_Video_Profile_1080_1920P_30,
  AliRTCSDK_Video_Profile_480_640P_15_1500Kb,
  AliRTCSDK_Video_Profile_Max,
};

enum AliRTCSDK_AudioEffect_Reverb_Mode{
    AliRTCSDK_AudioEffect_Reverb_Off = 0,
    AliRTCSDK_AudioEffect_Reverb_Vocal_I,
    AliRTCSDK_AudioEffect_Reverb_Vocal_II,
    AliRTCSDK_AudioEffect_Reverb_Bathroom,
    AliRTCSDK_AudioEffect_Reverb_Small_Room_Bright,
    AliRTCSDK_AudioEffect_Reverb_Small_Room_Dark,
    AliRTCSDK_AudioEffect_Reverb_Medium_Room,
    AliRTCSDK_AudioEffect_Reverb_Large_Room,
    AliRTCSDK_AudioEffect_Reverb_Church_Hall,
    AliRTCSDK_AudioEffect_Reverb_Cathedral,
    AliRTCSDK_AudioEffect_Reverb_MAX,
};

/**
 AudioEffect Reverb mode
 */
enum AliRTCSDK_AudioEffect_Reverb_Param_Type{
    AliRTCSDK_AudioEffect_Reverb_Room_Size = 0,
    AliRTCSDK_AudioEffect_Reverb_Pre_Delay,
    AliRTCSDK_AudioEffect_Reverb_Reverberance,
    AliRTCSDK_AudioEffect_Reverb_Hf_Damping,
    AliRTCSDK_AudioEffect_Reverb_Tone_Low,
    AliRTCSDK_AudioEffect_Reverb_Tone_High,
    AliRTCSDK_AudioEffect_Reverb_Dry_Gain,
    AliRTCSDK_AudioEffect_Reverb_Wet_Gain,
    AliRTCSDK_AudioEffect_Reverb_Type_Max,
};

enum AliRTCSDK_ScreenShareType {
  AliRTCSDK_ScreenShare_Desktop,
  AliRTCSDK_ScreenShare_Window,
};

void videoProfileToSize(enum AliRTCSDK_Video_Profile profile, int *preferWidth, int *preferHeight, int *preferFps);


enum AliRTCSDK_Log_Level {
  AliRTCSDK_LOG_DUMP,
  AliRTCSDK_LOG_DEBUG,
  AliRTCSDK_LOG_VERBOSE,
  AliRTCSDK_LOG_INFO,
  AliRTCSDK_LOG_WARNING,
  AliRTCSDK_LOG_ERROR,
  AliRTCSDK_LOG_FATAL,
  AliRTCSDK_LOG_NONE,
};

struct ALI_SDK_API AuthInfo {
  std::string channel;
  std::string user_id;
  std::string appid;
  std::string nonce;
  std::string token;
  std::string session;
  std::vector<std::string> gslb;
  std::vector<std::string> agent;
  unsigned long long timestamp;
};

struct ALI_SDK_API AliPublisherInfo {
  std::string  user_id;
  std::string  session;
  std::string  call_id;
  std::string  display;

  std::string stream;
  std::string video_track[MAX_VIDEO_TRACKS];
  std::string audio_track;
};

struct ALI_SDK_API AliScreenShareSourceInfo {
  intptr_t        sourceId;
  std::string     title;
};

struct ALI_SDK_API AliScreenShareRegion {
  float fOriginX = -1.f;
  float fOriginY = -1.f;
  float width = 0.f;
  float height = 0.f;
};

struct ALI_SDK_API AliScreenShareConfig {
  AliRTCSDK_ScreenShareType sourceType = AliRTCSDK_ScreenShare_Desktop;
  intptr_t sourceId = 0;
  std::string title = "";
  bool isShareByRegion = false;
  AliScreenShareRegion shareRegion;
};

struct ALI_SDK_API AliPublishConfig {
  bool enable_audio;
  bool enable_video_large;
  bool enable_video_small;
  bool enable_screen_share;
  bool enable_video_super;
  AliRTCSDK_Video_Profile video_track_profile[AliRTCSDK_VideoSource_Type_MAX] = { AliRTCSDK_Video_Profile_Default };
  AliScreenShareConfig screen_share_config;
};

struct ALI_SDK_API AliSubscribeConfig {
  char stream[MAX_MEDIA_STREAM_CHARS] = { 0 };
  char audio_track[MAX_MEDIA_STREAM_CHARS] = { 0 };
  char video_tracks[MAX_VIDEO_TRACKS][MAX_MEDIA_TRACK_CHARS] = { 0 };
};

enum ALI_SDK_API AliRTCSdk_Dislay_Mode {
  AliRTCSdk_Auto_Mode = 0,         //不做修改
  AliRTCSdk_FullOf_Mode = 1,          //铺满
  AliRTCSdk_FillBlcak_Mode = 2,       //填充黑边
  AliRTCSdk_Scale_Mode = 3,           //裁剪
  AliRTCSdk_Scroll_Mode = 4,           //滑动
};

enum ALI_SDK_API AliVideoRotation {
  kVideoRotation_0 = 0,
  kVideoRotation_90 = 90,
  kVideoRotation_180 = 180,
  kVideoRotation_270 = 270

};

enum ALI_SDK_API AliRTCSDK_Video_Scale {
  AliRTCSDK_Video_Scale_16_9 = 0,
  AliRTCSDK_Video_Scale_4_3,
};

struct ALI_SDK_API AliRendererConfig {
  void *displayView;
  int width;
  int height;
  AliRTCSdk_Dislay_Mode displayMode = AliRTCSdk_Auto_Mode;
  AliVideoRotation rotation{ AliVideoRotation::kVideoRotation_0 };
  AliRTCSDK_Video_Scale scale{ AliRTCSDK_Video_Scale_16_9 };
  bool flip;
};

struct ALI_SDK_API AliScrollViewConfig {
  float fOriginX;
  float fOriginY;
  float width;
  float height;
  AliRTCSdk_Dislay_Mode displayMode = AliRTCSdk_Auto_Mode;
  AliRTCSdk_VideoSource_Type videoSourceType = AliRTCSDK_VideoSource_Type_ScreenShare;
};

struct ALI_SDK_API AliRTCStreamConfig {
  const char *track_id;
  const char *stream_id;
};

enum AliRTCMediaConnectionState {
  AliRTC_MeidaConnection_New,
  AliRTC_MediaConnection_Checking,
  AliRTC_MediaConnection_Connected,
  AliRTC_MediaConnection_Completed,
  AliRTC_MediaConnection_Failed,
  AliRTC_MediaConnection_Disconnected,
  AliRTC_MediaConnection_Closed,
  AliRTC_MediaConnection_Max,
};

enum AliRTCMediaConnectionReConnectState {
  AliRTC_MeidaConnection_ReConnect_Connected = 0,
  AliRTC_MeidaConnection_ReConnect_ReConnecting,
  AliRTC_MeidaConnection_ReConnect_Recovery,
  AliRTC_MeidaConnection_ReConnect_Lost
};

struct ALI_SDK_API AliRTCCameraConfig {
  int preferWidth;
  int preferHeight;
  int preferFps;
};

struct ALI_SDK_API VideoFrame {
  const uint8_t *data_y;
  const uint8_t *data_u;
  const uint8_t *data_v;
  int32_t stride_y;
  int32_t stride_u;
  int32_t stride_v;
  int32_t width;
  int32_t height;
  int32_t rotation;
  int64_t capture_time;
};

struct ALI_SDK_API AudioFrame {
  int samples;
  void* buffer;
  int channel;
};

enum ALI_SDK_API TransportStatus {
  Transport_Excellent = 0,
  Transport_Good = 1,
  Transport_Poor = 2,
  Transport_Bad = 3,
  Transport_VeryBad = 4,
  Transport_Disconnected = 5,
  Transport_Unknow = 6,
};

enum ALI_SDK_API TransportType {
  Transport_Video_TYPE,
  Transport_Audio_TYPE,
  Transport_Mix_TYPE,
};

enum ALI_SDK_API ExternalDeviceState {
  External_Device_Add = 0,
  External_Device_Remove = 1,
};

enum ALI_SDK_API AliRTCSdk_AudioSource_Type {
  AliRTCSDK_Audio_Source_Pub = 0,
  AliRTCSDK_Audio_Source_Sub = 1,
  AliRTCSDK_Audio_Source_Raw_Data = 2,
  AliRTCSDK_Audio_Source_Volume = 3,
};

enum ALI_SDK_API AliRTCSdk_AudioNumChannel_Type {
  AliRTCSdk_AudioNumChanne_Mono = 0,
  AliRTCSdk_AudioNumChanne_Stereo = 1,
};

enum ALI_SDK_API AliRTCSdk_AudioSampleRate_Type {
  AliRTCSdk_AudioSampleRate_8000 = 0,
  AliRTCSdk_AudioSampleRate_11025 = 1,
  AliRTCSdk_AudioSampleRate_16000 = 2,
  AliRTCSdk_AudioSampleRate_22050 = 3,
  AliRTCSdk_AudioSampleRate_32000 = 4,
  AliRTCSdk_AudioSampleRate_44100 = 5,
  AliRTCSdk_AudioSampleRate_48000 = 6,
};

enum ALI_SDK_API AliRTCSdk_VideoFormat {
  AliRTCSdk_VideoFormat_UnKnown = -1,
  AliRTCSdk_VideoFormat_BGRA = 0,
  AliRTCSdk_VideoFormat_I420,
  AliRTCSdk_VideoFormat_NV21,
  AliRTCSdk_VideoFormat_NV12,
  AliRTCSdk_VideoFormat_MAX,
};

enum ALI_SDK_API AliRTCSdk_MediaRecordType {
  AliRTCSdk_RecordType_Audio = 0,
  AliRTCSdk_RecordType_Video = 1,
  AliRTCSdk_RecordType_Both = 2,
};

enum ALI_SDK_API AliRTCSdk_MediaRecordFormat {
  AliRTCSdk_RecordFile_AAC = 0,
  AliRTCSdk_RecordFile_WAV = 1,
  AliRTCSdk_RecordFile_MP4 = 2,
};

enum ALI_SDK_API AliRTCSdk_AudioRecording_Quality {
  AliRTCSdk_Audio_Recording_Low_Quality = 0,
  AliRTCSdk_Audio_Recording_Midium_Quality,
  AliRTCSdk_Audio_Recording_High_Quality,
};

enum ALI_SDK_API AliRTCSdk_Video_Recording_Quality {
  AliRTCSdk_Video_Recording_Default_Quality = 0,
};

enum ALI_SDK_API AliRTCSdk_MuteLocalAudioMode
{
  AliRTCSdk_MuteLocalAudioModeDefault = 0,
  AliRTCSdk_MuteAllLocalAudioMode = 1,
  AliRTCSdk_MuteOnlyMicAudioMode = 2,
  AliRTCSdk_MuteLocalAudioModeMAX
};

struct ALI_SDK_API AliParticipantInfo {
  std::string     user_id;
  std::string     session;
  std::string     user_name;
};

struct ALI_SDK_API AliSubscriberInfo {
  std::string     user_id;
  std::string     session;
  std::string     stream_type;
};

struct ALI_SDK_API AliUnPublisherInfo {
  std::string user_id;
  std::string session;
  std::string call_id;
};

struct ALI_SDK_API CameraConfig {
  int video_source{ 0 };    // 0 is back camera , 1 is front camera
  bool autoFocus{ true };
  bool flash{ false };
};

struct ALI_SDK_API StorageInterface {
  virtual std::string GetString(const std::string& key) const = 0;

  virtual void SetString(const std::string& key, const std::string& value) = 0;

  virtual ~StorageInterface() = default;
};

struct ALI_SDK_API AliRTCSdkStats {
  int64_t sent_kbitrate = 0;
  int64_t rcvd_kbitrate = 0;
  int64_t sent_bytes = 0;
  int64_t rcvd_bytes = 0;

  int64_t audio_rcvd_bytes = 0;
  int64_t video_rcvd_bytes = 0;
  int64_t audio_rcvd_kbitrate = 0;
  int64_t video_rcvd_kbitrate = 0;
  int64_t audio_sent_bytes = 0;
  int64_t video_sent_bytes = 0;
  int64_t audio_sent_kbitrate = 0;
  int64_t video_sent_kbitrate = 0;

  float sent_loss_rate = 0;
  float rcvd_loss_rate = 0;

  uint32_t user_num = 0;

  uint32_t system_cpu = 0;
  uint32_t app_cpu = 0;
  uint64_t call_duration = 0;
};

struct ALI_SDK_API AliRTCSdkLocalVideoStats {
  std::string local_call_id;
  std::string track_label;
  uint32_t sent_bitrate = 0;
  uint32_t sent_fps = 0;
  uint32_t encode_fps = 0;
};

struct ALI_SDK_API AliRTCSdkRemoteVideoStats {
  std::string remote_call_id;
  std::string track_label;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t decode_fps = 0;
  uint32_t render_fps = 0;
  uint32_t frozen_times = 0;
};

struct ALI_SDK_API AliRTCSdkAudioFrame {
  long dataPtr;
  int numOfSamples;
  int bytesPerSample;
  int numOfChannels;
  int samplesPerSec;
};

struct ALI_SDK_API AliRTCTransportInfo {
  std::string user_id;
  TransportStatus upQuality;
  TransportStatus downQuality;
};

struct ALI_SDK_API AliRTCDeviceInfo {
  std::string device_name;
  std::string device_id;
};

struct ALI_SDK_API AliRTCVideoFrame {
  AliRTCSdk_VideoFormat format = AliRTCSdk_VideoFormat_UnKnown;
  uint8_t *data = 0;
  size_t data_len = 0;
  int32_t stride_y = 0;
  int32_t stride_u = 0;
  int32_t stride_v = 0;
  int32_t width = 0;
  int32_t height = 0;
  int32_t rotation = 0;
  int64_t capture_time = 0;
};

struct ALI_SDK_API AliRTCAudioRecordConfig {
  int sampleRate;
  AliRTCSdk_AudioRecording_Quality quality;
  bool externalPcmCaptureRecording;
  bool externalPcmRenderRecording;
};

struct ALI_SDK_API AliRTCVideoRecordConfig {
  AliRTCSdk_Video_Recording_Quality quality;
};

class ALI_SDK_API Windows_AliRTCEventListener {
public:
  virtual void OnGslbResult(int result) = 0;

  virtual void OnJoinChannelResult(int result) = 0;

  virtual void OnLeaveChannelResult(int result) = 0;

  virtual void OnPublishResult(int result, const char *call_id) = 0;

  virtual void OnRepublishResult(int result, const char *call_id) = 0;

  virtual void OnUnpublishResult(int result, const char *call_id) = 0;

  virtual void OnSubscribeResult(int result, const char *call_id) = 0;

  virtual void OnSubscribeResult2(int result,
    const std::string &callid,
    const AliSubscribeConfig &reqConfig,
    const AliSubscribeConfig &curConfig) = 0;

  virtual void OnResubscribeResult(int result, const char* call_id) = 0;

  virtual void OnResubscribeResult2(int result,
    const std::string &callid,
    const AliSubscribeConfig &reqConfig,
    const AliSubscribeConfig &curConfig) = 0;

  virtual void OnUnsubscribeResult(int result, const char *call_id) = 0;

  virtual void OnWindowRenderReady(
    const char* callid,
    AliRTCSdk_VideoSource_Type type
  ) = 0;

  virtual void OnParticipantJoinNotify(const AliParticipantInfo joiner_info_list[], size_t count) = 0;

  virtual void OnParticipantLeaveNotify(const AliParticipantInfo leaver_list[], size_t count) = 0;

  virtual void OnParticipantPublishNotify(AliPublisherInfo publisher_list[], size_t count) = 0;

  virtual void OnParticipantUnpublishNotify(AliUnPublisherInfo unpublisher_list[], size_t count) = 0;

  virtual void OnParticipantSubscribeNotify(const AliSubscriberInfo subscriber_info_list[], size_t count) = 0;

  virtual void OnParticipantUnsubscribeNotify(const AliParticipantInfo remote_publish_call_id_list[], size_t count) = 0;

  virtual void OnCollectStats(const char *call_id,
    const std::vector<std::map<std::string, std::string>>& stats_set) = 0;

  virtual void OnResolutionChange(const std::string &call_id, const std::string &stream_label, const std::string &track_label,
    int width, int height) = 0;

  virtual void OnFirstPacketSent(const std::string& call_id, const std::string& stream_label, const std::string& track_label, int time_cost_ms) = 0;
  virtual void OnFirstPacketReceived(const std::string& call_id, const std::string& stream_label, const std::string& track_label, int time_cost_ms) = 0;

  virtual void OnWarning(int code, void* params) = 0;

  virtual void OnError(int code, void* params) = 0;

  virtual void OnTransportStatusChange(const char *call_id,
    TransportType event, TransportStatus status) = 0;

  virtual void OnConnectionChange(
    AliRTCMediaConnectionReConnectState mediaConState) = 0;

  virtual void OnLogMessage(const std::string& message) = 0;

  virtual void OnMessage(
    const std::string &tid,
    const std::string &content_type,
    const std::string &content) = 0;

  virtual void OnBye(int32_t code) = 0;

  virtual void OnUplinkChannelMessage(int result,
    const std::string &content_type, const std::string &content) = 0;

  virtual void OnSDKDestroyed() = 0;

  virtual void OnExternalDeviceStateChange(const std::string& deviceName, ExternalDeviceState state) = 0;

  virtual void OnRenderData(const char* callId, AliRTCSdk_VideoSource_Type type, void* dataPtr, int format, int width, int height, int stride, int rotation, long extraData) = 0;

  virtual void OnUpdateRoleNotify(const AliRTCSDK_Client_Role old_role, const AliRTCSDK_Client_Role new_role) = 0;

  virtual void OnAliRTCStats(const AliRTCSdkStats& stats) = 0;

  virtual void OnAliRTCLocalVideoStats(
    const AliRTCSdkLocalVideoStats& local_video_stats) {};

  virtual void OnAliRTCRemoteVideoStats(
    const AliRTCSdkRemoteVideoStats& remote_video_stats) {};

  virtual void OnNetworkQualityChanged(const std::vector<AliRTCTransportInfo>& network_quality) = 0;

  virtual void OnAudioDataCallback(AliRTCSdk_AudioSource_Type type, AliRTCSdkAudioFrame audio_frame) = 0;

  virtual void OnAudioVolumeCallback(AliRTCSdk_AudioSource_Type type, int volume, const std::string& call_id, int audiostate) = 0;

  virtual void onLastmileDetectResultWithQuality(TransportStatus networkQuality) {};

  virtual void OnVideoSampleCallback(const std::string& call_id, AliRTCSdk_VideoSource_Type type, AliRTCVideoFrame* video_frame) {};
  
  virtual void OnLiveStreamingSignalingResult(
    int result) {};

  virtual void OnLiveStreamingTrackAdded(
    const std::string& call_id,
    const std::string& stream_label,
    const std::string& track_label) {};

  virtual void OnMediaRecordEvent(int event, const char* filePath) {};

  virtual ~Windows_AliRTCEventListener() = default;
};

class ALI_SDK_API  Windows_MediaDeviceTestEventListener {
public:
  virtual void OnAudioDeviceRecordLevel(int level) = 0;
  virtual void OnAudioDevicePlayoutLevel(int level) = 0;
  virtual void OnAudioDevicePlayoutEnd() = 0;
  virtual ~Windows_MediaDeviceTestEventListener() = default;
};

class ALI_SDK_API Windows_MediaDeviceTestInterface {
public:
  virtual ~Windows_MediaDeviceTestInterface() {}
  virtual int StartTestAudioRecord(const char * deviceName, int32_t nTimeInv) = 0;
  virtual int StartTestAudioRecordById(const char *deviceId, int32_t nTimeInv) = 0;
  virtual int StopTestAudioRecord() = 0;
  virtual int StartTestAudioPlayout(const char * deviceName, int32_t nTimeInv, const char * wavFile, int loopCycles = 0) = 0;
  virtual int StartTestAudioPlayoutById(const char *deviceId, int32_t nTimeInv, const char *wavFile, int loopCycles = 0) = 0;
  virtual int StopTestAudioPlayout() = 0;
  virtual int SetAudioCaptureDevice(const char * deviceName) = 0;
  virtual int SetAudioPlayoutDevice(const char * deviceName) = 0;
  virtual void Release() = 0;
};

class ALI_SDK_API Windows_MediaInputStreamInterface
{
public:
	// API for system audio recording
	virtual int EnableSystemAudioRecording(bool flag) = 0;
	virtual bool IsSystemAudioRecording() = 0;
	virtual int SetSystemAudioRecordingVolScal(int volScal) = 0;
	virtual int GetSystemAudioRecordingVolScal() = 0;

	virtual int AddAudioStream(size_t num_channels, size_t sample_rate, const void* config) = 0;
    virtual int RemoveAudioStream(int stream_handle) = 0;
    virtual int SendAudioData(int stream_handle, size_t sample_length, const void* audio_samples) = 0;
    virtual int ClearAudioCache(int stream_handle) = 0;
    virtual int UpdateAudioStreamConfig(int stream_handle, const void* config) = 0;
    virtual int SetStreamVolScal(int stream_handle, int volScal) = 0;
    virtual int GetStreamVolScal(int stream_handle) = 0;

    //virtual int AddVideoStream() = 0;
    //virtual int RemoveVideoStream(int stream_handle) = 0;
    //virtual int SendVideoData(int stream_handle, const VideoInputInfo *video_input_info) = 0;

    virtual void Reset() = 0;
	virtual void Release() = 0;
};

class ALI_SDK_API Windows_AliRTCSDKInterface {

public:

  static Windows_AliRTCSDKInterface* Create(
    Windows_AliRTCEventListener *listener,
    const std::string& extras = "",
    StorageInterface* storage = nullptr);

  static void Destroy(Windows_AliRTCSDKInterface *ali_interface);

  static const char *GetSDKVersion();

  static int SetLogRootPath(const std::string &logRootPath);

  static void UploadLog();

  static void UploadChannelLog();

  static int Log(const std::string &file, int line,
    AliRTCSDK_Log_Level sev, const std::string &tag,
    const std::string &log);

  static int ChangeLogLevel(AliRTCSDK_Log_Level level);

  static AliRTCSDK_Log_Level GetLogLevel();

  virtual ~Windows_AliRTCSDKInterface() {};

  virtual int SetChannelProfile(const AliRTCSDK_Channel_Profile channel_profile) = 0;

  virtual int SetClientRole(const AliRTCSDK_Client_Role client_role) = 0;

  virtual int Gslb(
    const std::string& channel,
    const std::string& user_id,
    const std::string& appid,
    const std::string& nonce,
    const std::string& token,
    const std::string& session,
    const std::vector<std::string>& gslb,
    unsigned long long timestamp) = 0;

  virtual int JoinChannel(const char *display_name) = 0;

  virtual int JoinChannel(const AuthInfo &auth_info, const char *display_name) = 0;

  virtual int LeaveChannel() = 0;

  virtual int LeaveChannel(unsigned long timeout_ms) = 0;

  virtual int OpenCamera() = 0;
  virtual int OpenCameraEx(AliRTCCameraConfig &config) = 0;

  virtual int CloseCamera() = 0;

  virtual int AddLocalDisplayWindow(AliRTCSdk_VideoSource_Type type, const AliRendererConfig& config) = 0;

  virtual int RemoveLocalDisplayWindow(AliRTCSdk_VideoSource_Type type) = 0;

  virtual int Publish(const AliPublishConfig& publish_config) = 0;

  virtual int Republish(const AliPublishConfig& publish_config) = 0;

  virtual int Unpublish() = 0;

  virtual int Subscribe(
    const char *call_id,
    const AliSubscribeConfig& conf) = 0;

  virtual int Resubscribe(
    const char *call_id,
    const AliSubscribeConfig& conf) = 0;

  virtual int Unsubscribe(const char *call_id) = 0;

  virtual int AddRemoteDisplayWindow(const char *call_id, const std::string& label, const AliRendererConfig& config) = 0;

  virtual int RemoveRemoteDisplayWindow(const char *call_id, const std::string& label) = 0;

  virtual int UpdateDisplayWindow(const AliRendererConfig &config) = 0;

  virtual int UpdateScrollView(const AliScrollViewConfig &config) = 0;

  virtual int EnableLocalAudio(bool flag, AliRTCSdk_MuteLocalAudioMode mode = AliRTCSdk_MuteLocalAudioModeDefault) = 0;

  virtual int MuteAllRemoteAudio(bool mute) = 0;

  virtual int EnableLocalVideo(AliRTCSdk_VideoSource_Type type, bool flag) = 0;

  virtual int EnableRemoteAudio(const char *call_id, bool mute) = 0;

  virtual int EnableRemoteVideo(const char *call_id, const std::string& label, bool flag) = 0;

  virtual int EnableAudioPlay(bool enable) = 0;

  virtual std::vector<AliRTCDeviceInfo> EnumerateVideoCaptureDevices() = 0;

  virtual int SetVideoCaptureDeviceByName(const char* capture_name) = 0;

  virtual int SetVideoCaptureDeviceById(const std::string &deviceId) = 0;

  virtual std::string GetCurrentVideoCaptureId() = 0;

  virtual int GetCurrentVideoCaptureName(char* device_name, size_t* size) = 0;

  virtual std::vector<AliRTCDeviceInfo> EnumerateAudioCaptureDevices() = 0;

  virtual int SetAudioCaptureDeviceByName(const char* device_name, bool shouldStart = false) = 0;

  virtual int SetAudioCaptureDeviceById(const char* deviceId) = 0;

  virtual int GetCurrentAudioCaptureName(char* capture_name, size_t* size) = 0;

  virtual std::string GetCurrentAudioCaptureId() = 0;

  virtual std::vector<AliRTCDeviceInfo> EnumerateAudioRenderDevices() = 0;

  virtual int SetAudioRenderDeviceByName(const std::string &render_name, bool shouldStart = false) = 0;

  virtual int SetAudioRenderDeviceById(const std::string &deviceId) = 0;

  virtual int GetCurrentAudioRenderName(char* render_name, size_t* size) = 0;

  virtual std::string GetCurrentAudioRenderId() = 0;

  virtual Windows_MediaDeviceTestInterface * CreateMediaDeviceTestInterface(Windows_MediaDeviceTestEventListener * pMediaDeviceEventListener) = 0;

  virtual int respondMessageNotification(
    const std::string &tid,
    const std::string &content_type,
    const std::string &content) = 0;

  virtual int UplinkChannelMessage(
    const std::string &content_type,
    const std::string &content) = 0;

  virtual int SetRecordingVolume(int volume) = 0;
  virtual int SetPlayoutVolume(int volume) = 0;

  virtual int SetVolumeCallbackIntervalMs(int interval, int smooth, int report_vad) = 0;
  virtual int SetAudioEffectReverbMode(const AliRTCSDK_AudioEffect_Reverb_Mode mode) = 0;
  virtual int SetAudioEffectReverbParamType(const AliRTCSDK_AudioEffect_Reverb_Param_Type type, float value) = 0;

  virtual int SetAudioVolume(int audio_device, int volume) = 0;

	virtual int GetAudioVolume(int audio_device) = 0;

	virtual Windows_MediaInputStreamInterface* GetMediaInputStreamInterface() = 0;

  virtual AliRTCSDK_Error_Event GetErrorType(int code) = 0;

  virtual int SetTraceId(const std::string& trace_id) = 0;

  virtual std::string getMediaInfoWithKeys(const std::string& call_id,
      const std::string& track_id, const std::string key_list[], int length) = 0;

  virtual int EnableBackGroundRecording(bool flag) = 0;

  virtual bool IsBackGroundRecording() = 0;

  virtual int EnableBackGroundPlayout(bool flag) = 0;

  virtual bool IsBackGroundPlayout() = 0;

  virtual int RegistVideoRenderDataObserver(const std::string &call_id, AliRTCSdk_VideoSource_Type type, const AliRendererConfig& config) = 0;

  virtual int UnRegistVideoRenderDataObserver(const std::string &call_id, AliRTCSdk_VideoSource_Type type) = 0;

  virtual int UpdateVideoRenderDataObserver(const std::string &call_id, AliRTCSdk_VideoSource_Type type, const AliRendererConfig &config) = 0;

  virtual int RegistLiveStreamingRenderDataObserver(const std::string &call_id, std::string &stream_label,
    std::string &track_label, AliRendererConfig config) = 0;

  virtual int UnRegistLiveStreamingRenderDataObserver() = 0;

  virtual std::vector<AliScreenShareSourceInfo> GetScreenShareSourceList(AliRTCSDK_ScreenShareType source_type) = 0;

  virtual int GetDesktopResolution(AliScreenShareSourceInfo source_info, int& width, int& height) = 0;

  virtual void SetAudioNumChannel(AliRTCSdk_AudioNumChannel_Type type) = 0;

  virtual void SetAudioSampleRate(AliRTCSdk_AudioSampleRate_Type type) = 0;

  virtual void SubscribeAudioData(AliRTCSdk_AudioSource_Type source_type) = 0;

  virtual void UnsubscribeAudioData(AliRTCSdk_AudioSource_Type source_type) = 0;

  virtual int startLastmileDetect() = 0;

  virtual int stopLastmileDetect() = 0;

  virtual int SetExternalVideoSource(bool enable, bool use_texture, AliRTCSdk_VideoSource_Type source_type) = 0;

  virtual int PushExternalVideoFrame(AliRTCVideoFrame *frame, AliRTCSdk_VideoSource_Type source_type) = 0;

  virtual void AddVideoDataCallbackObserver() = 0;

  virtual void RemoveVideoDataCallbackObserver() = 0;

  virtual int SetExternalAudioSource(bool enable, unsigned int sampleRate, unsigned int channelsPerFrame) = 0;

  virtual int PushExternalAudioFrameRawData(const void* audio_samples, unsigned int sample_length, long long timestamp) = 0;

  virtual int SetMixedWithMic(bool mixed) = 0;

  virtual int SetExternalAudioVolume(int volume) = 0;

  virtual int GetExternalAudioVolume() = 0;

  virtual int SetExteranlAudioRender(bool enable, unsigned int sampleRate, unsigned int channelsPerFrame) = 0;

  virtual int PushExternalAudioRenderRawData(const void* audioSamples, unsigned int sampleLength, unsigned int sampleRate, unsigned int channelsPerFrame, long long timestamp) = 0;

  virtual int StartLiveStreaming(const AuthInfo& auth_info) = 0;

  virtual int StopLiveStreaming() = 0;

  virtual int AddLiveDisplayWindow(const char* call_id,
    const char* stream_label,
    const char* track_label,
    AliRendererConfig config) = 0;

  virtual int RemoveLiveDisplayWindow() = 0;

  virtual int UpdateLiveDisplayWindows(const char* call_id, const AliRendererConfig &config) = 0;

  virtual bool StartRecord(AliRTCSdk_MediaRecordType recordType, AliRTCSdk_MediaRecordFormat recordFormat, std::string& filePath, AliRTCAudioRecordConfig& audioConfig, AliRTCVideoRecordConfig& videoConfig) = 0;

  virtual void StopRecord() = 0;

  virtual int SetExternalAudioRenderVolume(int volume) = 0;
  
  virtual int GetExternalAudioRenderVolume() = 0;
};

}
