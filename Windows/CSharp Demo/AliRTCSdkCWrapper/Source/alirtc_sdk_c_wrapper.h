#ifndef AliRTCSDKCWrapper_h
#define AliRTCSDKCWrapper_h
#include <stdio.h>

#if defined(_WIN32)
#if defined(ALIARTC_EXPORT)
#define ALIRTCSDK_API extern "C" __declspec(dllexport)
#else
#define ALIRTCSDK_API extern "C" __declspec(dllimport)
#endif

#define ALIRTCSDK_CALL __cdecl

#elif defined(__APPLE__)
#include "TargetConditionals.h"
#define ALIRTCSDK_API __attribute__((visibility("default"))) extern "C"
#define ALIRTCSDK_CALL
#if TARGET_OS_IPHONE
#define __IOS__
#elif TARGET_OS_MAC
#define __OSX__
#endif

#elif defined(__ANDROID__) || defined(__linux__)
#define ALIRTCSDK_API extern "C" __attribute__((visibility("default")))
#define ALIRTCSDK_CALL
#else
#define ALIRTCSDK_API extern "C"
#define ALIRTCSDK_CALL
#endif

typedef enum _AliRTCVideoTrack {
    VIDEO_TRACK_NONE = 0,
    VIDEO_TRACK_CAMERA,
    VIDEO_TRACK_SCREEN,
    VIDEO_TRACK_BOTH,
}AliRTCVideoTrack;

typedef enum _AliRTCAudioTrack {
    AUDIO_TRACK_NONE = 0,
    AUDIO_TRACK_MIC,
}AliRTCAudioTrack;

typedef enum _AliRTCNetworkQuality {
    NETWORK_QUALITY_EXCELLENT = 0,
    NETWORK_QUALITY_GOOD,
    NETWORK_QUALITY_POOR,
    NETWORK_QUALITY_BAD,
    NETWORK_QUALITY_VERY_BAD,
    NETWORK_QUALITY_VERY_DISCONNECT,
    NETWORK_QUALITY_UNKNOWN,
}AliRTCNetworkQuality;

typedef enum _AliRTCRenderMirror {
    RENDER_MIRROR_FRONT_PREVIEW_ENABLED = 0,
    RENDER_MIRROR_ALL_ENABLED,
    RENDER_MIRROR_ALL_DISABLED,
}AliRTCRenderMirror;

typedef enum _AliRTClogLevel{
    LOG_LEVEL_DUMP = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_NONE,
}AliRTClogLevel;

typedef enum _AliRTCExternalDeviceState {
    EXTERNAL_DEVICE_ADD = 0,
    EXTERNAL_DEVICE_REMOVE = 1,
}AliRTCExternalDeviceState;

typedef enum _AliRTCChannelProfile {
    CHANNEL_PROFILE_COMMUNICATION = 0,
    CHANNEL_PROFILE_INTERACTIVELIVE = 1,
    CHANNEL_PROFILE_CDNLIVE = 2,
}AliRTCChannelProfile;

typedef enum _AliRTCClientRole {
    CLIENT_ROLE_INTERACTIVE = 0,
    CLIENT_ROLE_LIVE = 1,
}AliRTCClientRole;

typedef enum _AliRTCAudioSource {
  AliRTCAudiosourcePub = 0,
  AliRTCAudiosourceSub = 1,
  AliRTCAudiosourceRawData = 2,
}AliRTCAudioSource;

typedef enum _AliRTCVideoSource {
  VIDEO_SOURCE_CAMERA = 0,
  VIDEO_SOURCE_SCREENSHARE = 1,
}AliRTCVideoSource;

typedef enum _AliRTCVideoFormat {
  VIDEO_FORMAT_UNKNOW = -1,
  VIDEO_FORMAT_BGRA = 0,
  VIDEO_FORMAT_I420,
  VIDEO_FORMAT_NV21,
  VIDEO_FORMAT_NV12,
}AliRTCVideoFormat;


typedef enum _AliRTCConvertVideoFormat {
  CONVERT_VIDEO_FORMAT_UNKNOW,
  CONVERT_VIDEO_FORMAT_I420,
  CONVERT_VIDEO_FORMAT_ABGR,
}AliRTCConvertVideoFormat;

typedef enum _AliRTCBufferType {
  BUFFER_TYPE_RAW_DATA = 0,
  BUFFER_TYPE_CVPIXEL_BUFFER,
  BUFFER_TYPE_TEXTURE,
}AliRTCBufferType;

typedef enum _AliRTCMuteLocalAudioMode
{
  AliRTCMuteLocalAudioModeDefault = 0,
  AliRTCMuteLocalAudioModeMuteAll,
  AliRTCMuteLocalAudioModeMuteOnlyMic,
}AliRTCMuteLocalAudioMode;

typedef struct _AliRTCTextureInfo {
    int textureId;
    int flip;
}AliRTCTextureInfo;

typedef struct _AliRTCVideoDataSample {
  AliRTCVideoFormat format;
  AliRTCBufferType bufferType;
  long dataLength = 0;
  void* pixelBuffer = 0;
  void* dataPtr = 0;
  void* dataYPtr = 0;
  void* dataUPtr = 0;
  void* dataVPtr = 0;
  int strideY = 0;
  int strideU = 0;
  int strideV = 0;
  int height = 0;
  int width = 0;
  int rotation = 0;
  int stride = 0;
  long long timeStamp = 0;
  int textureId = 0;
  float transformMatrix[16] = { 0 };
}AliRTCVideoDataSample;

/**
* @brief 会话数据统计信息
*/
typedef struct {
  long long sentKBitrate = 0;
  long long rcvdKBitrate = 0;
  long long sentBytes = 0;
  long long rcvdBytes = 0;
  long long videoRcvdKBitrate = 0;
  long long videoSentKBitrate = 0;
  unsigned int systemCpu = 0;
  unsigned int appCpu = 0;
  unsigned long long callDuration = 0;
  long long sentLossRate = 0;
  long long sentLossPkts = 0;
  long long sentExpectedPkts = 0;
  long long rcvdLossRate = 0;
  long long rcvdLossPkts = 0;
  long long rcvdExpectedPkts = 0;
  long long lastmileDelay = 0;
} AliRTCStats;
/**
* @brief 用户离线原因
*/
typedef enum {
  AliRTCUserOfflineQuit = 0,              /* 用户主动离开 */
  AliRTCUserOfflineDropped = 1,           /* 因过长时间收不到对方数据包，超时掉线 */
  AliRTCUserOfflineBecomeAudience = 2,    /* 用户身份从主播切换为观众时触发 */
} AliRTCUserOfflineReason;
/**
* @brief 推流状态
*/
typedef enum {
  AliRTCStatsPublishIdle = 0,  /* 初始状态 */
  AliRTCStatsNoPublish = 1,    /* 未推流 */
  AliRTCStatsPublishing = 2,   /* 推流中 */
  AliRTCStatsPublished = 3,    /* 已推流 */
} AliRTCPublishState;

/**
* @brief 订阅状态
*/
typedef enum {
  AliRTCStatsSubscribeIdle = 0,  /* 初始状态 */
  AliRTCStatsNoSubscribe = 1,    /* 未订阅 */
  AliRTCStatsSubscribing = 2,    /* 订阅中 */
  AliRTCStatsSubscribed = 3,     /* 已订阅 */
} AliRTCSubscribeState;

/**
* @brief 相机流类型
*/
typedef enum {
  AliRTCVideoStreamTypeNone = 0,
  AliRTCVideoStreamTypeHigh = 1,
  AliRTCVideoStreamTypeLow = 2,
} AliRTCVideoStreamType;

/**
* @brief 设备信息
*/
typedef struct {
  const char* deviceName;
  const char* deviceID;
} AliRTCDeviceInfo;

/**
* @brief 音量信息
*/
typedef struct {
  const char* uid = nullptr;  /* 用户id */
  bool speechState = 0; /* 语音状态，1表示当前在说话，0表示当前未说话 */
  int volume = 0;       /* 音量 */
  int sumVolume = 0;   /* 回调周期内累计的音量, 用于判别当前说话人 */
} AliRTCUserVolumeInfo;

/**
* @brief 音频裸数据
*/
typedef struct {
  void* dataPtr = 0;
  int numOfSamples = 0;
  int bytesPerSample = 0;
  int numOfChannels = 0;
  int sampleRate = 0;
  int samplesPerSec = 0;
} AliRTCAudioRawData;

/**
* @brief 视频旋转角度
*/
typedef enum {
  AliRTCRotationMode_NoChange = -1,
  AliRTCRotationMode_0 = 0,
  AliRTCRotationMode_90 = 90,
  AliRTCRotationMode_180 = 180,
  AliRTCRotationMode_270 = 270,
} AliRTCRotationMode;
/**
* @brief 视频编码属性设置
*/
struct AliRTCVideoEncoderConfiguration {
  int width;
  int height;
  int frameRate;
  int bitrate;
  int orientationMode;
  int mirrorMode;
  AliRTCRotationMode rotationMode;
};

/**
* @brief 桌面分享类型
*/
typedef enum {
  AliRTCScreenShareDesktop,      /* 桌面分享 */
  AliRTCScreenShareWindow,       /* 窗口分享 */
} AliRTCScreenShareType;

/**
* @brief 录制类型
*/
typedef enum {
  AliRTCRecordTypeAudio = 0,
  AliRTCRecordTypeVideo,
  AliRTCRecordTypeBoth,
} AliRTCRecordType;

/**
* @brief 录制格式
*/
typedef enum {
  AliRTCRecordFormatAAC = 0,
  AliRTCRecordFormatWAV,
  AliRTCRecordFormatMP4,
} AliRTCRecordFormat;

/**
* @brief 音频采样率类型
*/
typedef enum {
  AliRTCAudioSampleRate_8000 = 0,
  AliRTCAudioSampleRate_11025 = 1,
  AliRTCAudioSampleRate_16000 = 2,
  AliRTCAudioSampleRate_22050 = 3,
  AliRTCAudioSampleRate_32000 = 4,
  AliRTCAudioSampleRate_44100 = 5,
  AliRTCAudioSampleRate_48000 = 6,
} AliRTCAudioSampleRate;

/**
* @brief 录制音频质量
*/
typedef enum {
  AliRTCAudioQualityLow = 0,
  AliRTCAudioQualityMidium,
  AliRTCAudioQualityHigh,
} AliRTCAudioQuality;

/**
* @brief 录制视频分辨率
*/
typedef struct AliRTCVideoRecordCanvasConfig {
  int canvasWidth;
  int canvasHeight;
}AliRTCVideoRecordCanvasConfig;

/**
* @brief 录制视频模板
*/
typedef struct AliRTCRecordTemplate {
  AliRTCRecordType recordType;
  AliRTCRecordFormat recordFormat;
  //audio
  AliRTCAudioSampleRate sampleRate = AliRTCAudioSampleRate_16000;
  AliRTCAudioQuality audioQuality = AliRTCAudioQualityHigh;
  bool externalPcmCaptureRecording = false;
  bool externalPcmRenderRecording = true;
  //video
  AliRTCVideoRecordCanvasConfig canvas;
  bool isFragment = true;
  int fps = 15;
  int bitrate = 500;
}AliRTCRecordTemplate;

/**
* @brief 录制视频坐标值
*/
typedef struct AliRTCRecordVideoRational {
  unsigned int numerator;
  unsigned int denominator;
}AliRTCRecordVideoRational;

/**
* @brief 录制视频坐标
*/
typedef struct AliRTCRecordVideoRectangle {
  AliRTCRecordVideoRational left;// percentage
  AliRTCRecordVideoRational top;
  AliRTCRecordVideoRational width;
  AliRTCRecordVideoRational height;
}AliRTCRecordVideoRectangle;

/**
* @brief 录制视频用户布局
*/
typedef struct AliRTCRecordVideoRegion {
  char* userId;
  AliRTCVideoSource sourceType;
  AliRTCRecordVideoRectangle area;
}AliRTCRecordVideoRegion;

/**
* @brief 录制视频布局
*/
typedef enum {
  AliRTCRecordVideoLayoutModeGrid = 0,
  AliRTCRecordVideoLayoutModeSpeaker,
  AliRTCRecordVideoLayoutModeUnique,
  AliRTCRecordVideoLayoutModeCustom,
}AliRTCRecordVideoLayoutMode;

/**
* @brief 录制视频背景色
*/
typedef struct AliRTCRecordVideoBgColor {
  unsigned char r;
  unsigned char g;
  unsigned char b;
}AliRTCRecordVideoBgColor;


//callback define
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnJoinChannelNotify)(int result, const char *channel, int elapsed);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnLeaveChannelResultNotify)(int result, AliRTCStats stats, const char* channelId);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnAudioPublishStateChangedNotify)(AliRTCPublishState oldState, AliRTCPublishState newState, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnVideoPublishStateChangedNotify)(AliRTCPublishState oldState, AliRTCPublishState newState, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnDualStreamPublishStateChangedNotify)(AliRTCPublishState oldState, AliRTCPublishState newState, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnScreenSharePublishStateChangedNotify)(AliRTCPublishState oldState, AliRTCPublishState newState, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnAudioSubscribeStateChangedNotify)(const char *uid, AliRTCSubscribeState oldState, AliRTCSubscribeState newState, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnVideoSubscribeStateChangedNotify)(const char *uid, AliRTCSubscribeState oldState, AliRTCSubscribeState newState, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnScreenShareSubscribeStateChangedNotify)(const char *uid, AliRTCSubscribeState oldState, AliRTCSubscribeState newState, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnSubscribeStreamTypeChangedNotify)(const char *uid, AliRTCVideoStreamType oldStreamType, AliRTCVideoStreamType newStreamType, int elapseSinceLastState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnRemoteUserOnLineNotify)(const char *uid, int elapsed, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnRemoteUserOffLineNotify)(const char *uid, AliRTCUserOfflineReason reason, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnRemoteTrackAvailableNotify)(const char* userId, int audioTrack, int videoTrack, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnNetworkQualityChangedNotify)(const char* userId, AliRTCNetworkQuality upQuality, AliRTCNetworkQuality downQuality, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnConnectionLostNotify)(const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnTryToReconnectNotify)(const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnConnectionRecoveryNotify)(const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnByeNotify)(int code, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnOccurWarningNotify)(int code, const char *msg, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnOccurErrorNotify)(int error, const char *msg, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnAudioDeviceStateChangedNotify)(const AliRTCDeviceInfo& deviceInfo, int deviceType, int deviceState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnVideoDeviceStateChangedNotify)(const AliRTCDeviceInfo& deviceInfo, int deviceType, int deviceState, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnUpdateRoleNotify)(int oldRole, int newRole, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnAudioVolumeCallback)(const AliRTCUserVolumeInfo* volumeInfo, int volumeInfoCount, int totalVolume, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnAudioDeviceRecordLevel)(int level, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnAudioDevicePlayoutLevel)(int level, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnAudioDevicePlayoutEnd)(const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnLastmileDetectResultWithQuality)(AliRTCNetworkQuality networkQuality, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnMediaRecordEvent)(int event, const char* filePath);

typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnRawDataAudioFrame)(AliRTCAudioRawData audioRawData, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnPublishAudioFrame)(AliRTCAudioRawData audioRawData, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnSubscribeAudioFrame)(AliRTCAudioRawData audioRawData, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnPlaybackAudioFrameBeforeMixing)(const char *uid, AliRTCAudioRawData audioRawData, const char *channel);

typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnCaptureVideoSample)(AliRTCVideoSource sourceType, AliRTCVideoDataSample* videoSample, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnPreEncodeVideoSample)(AliRTCVideoSource sourceType, AliRTCVideoDataSample* videoSample, const char *channel);
typedef void(ALIRTCSDK_CALL *ALIRTC_FUNC_OnRemoteVideoVideoSample)(const char* userId, AliRTCVideoSource sourceType, AliRTCVideoDataSample* videoSample, const char *channel);

/********************************API***********************************/
ALIRTCSDK_API void  AliRtcCreateEngine(const char* extras);
ALIRTCSDK_API void  AliRtcDestroyEngine();
ALIRTCSDK_API const char *  AliRtcGetSdkVersion();
ALIRTCSDK_API int  AliRtcStartLastmileDetect();
ALIRTCSDK_API int  AliRtcStopLastmileDetect();
ALIRTCSDK_API int  AliRtcPublishLocalVideoStream(bool enabled, const char *channel);
ALIRTCSDK_API bool  AliRtcIsLocalVideoStreamPublished(const char *channel);
ALIRTCSDK_API int  AliRtcPublishLocalAudioStream(bool enabled, const char *channel);
ALIRTCSDK_API bool  AliRtcIsLocalAudioStreamPublished(const char *channel);
ALIRTCSDK_API int  AliRtcPublishLocalDualStream(bool enabled, const char *channel);
ALIRTCSDK_API bool  AliRtcIsDualStreamPublished(const char *channel);
ALIRTCSDK_API int  AliRtcSetRemoteVideoStreamType(const char* uid, int streamType, const char *channel);
ALIRTCSDK_API int  AliRtcSetRemoteDefaultVideoStreamType(int streamType, const char *channel);
ALIRTCSDK_API int  AliRtcSetDefaultSubscribeAllRemoteAudioStreams(bool sub, const char *channel);
ALIRTCSDK_API int  AliRtcSubscribeAllRemoteAudioStreams(bool sub, const char *channel);
ALIRTCSDK_API int  AliRtcSubscribeRemoteAudioStream(const char* uid, bool sub, const char *channel);
ALIRTCSDK_API int  AliRtcSetDefaultSubscribeAllRemoteVideoStreams(bool sub, const char *channel);
ALIRTCSDK_API int  AliRtcSubscribeAllRemoteVideoStreams(bool sub, const char *channel);
ALIRTCSDK_API int  AliRtcSubscribeRemoteVideoStream(const char* uid, AliRTCVideoTrack track, bool sub, const char *channel);
ALIRTCSDK_API int  AliRtcSetAudioOnlyMode(bool audioOnly, const char *channel);
ALIRTCSDK_API bool  AliRtcIsAudioOnlyMode(const char *channel);
ALIRTCSDK_API int  AliRtcJoinChannel(const char* channel, const char* userId, const char* appId,
  const char* nonce, const char* token, const char* role, const char* gslbArray,
  const char* agentArray, long long timestamp, const char* userName);
ALIRTCSDK_API int  AliRtcLeaveChannel(const char *channel);
ALIRTCSDK_API int  AliRtcStartPreview();
ALIRTCSDK_API int  AliRtcStopPreview();
ALIRTCSDK_API int  AliRtcMuteLocalCamera(bool mute, int track, const char *channel);
ALIRTCSDK_API void  AliRtcSetVideoEncoderConfiguration(const AliRTCVideoEncoderConfiguration& config);
ALIRTCSDK_API int  AliRtcMuteLocalMic(bool mute, AliRTCMuteLocalAudioMode mode, const char *channel);
ALIRTCSDK_API int  AliRtcMuteAllRemoteAudio(bool mute, const char *channel);
ALIRTCSDK_API int  AliRtcMuteRemoteAudio(const char* userId, bool mute, const char *channel);
ALIRTCSDK_API const char*  AliRtcGetOnlineRemoteUsers(const char *channel);
ALIRTCSDK_API bool  AliRtcIsUserOnline(const char* userId, const char *channel);
ALIRTCSDK_API void  AliRtcStartAudioCapture();
ALIRTCSDK_API void  AliRtcStopAudioCapture();
ALIRTCSDK_API void  AliRtcStartAudioPlayer();
ALIRTCSDK_API void  AliRtcStopAudioPlayer();
ALIRTCSDK_API void  AliRtcSetLogLevel(AliRTClogLevel logLevel);
ALIRTCSDK_API int  AliRtcSetLogDirPath(const char* logDirPath);
ALIRTCSDK_API void  AliRtcUploadLog();
ALIRTCSDK_API bool  AliRtcIsCameraOn(const char *channel);
ALIRTCSDK_API bool  AliRtcIsInCall(const char *channel);
ALIRTCSDK_API void  AliRtcSetH5CompatibleMode(bool comp);
ALIRTCSDK_API bool  AliRtcGetH5CompatibleMode();
ALIRTCSDK_API int  AliRtcSetChannelProfile(AliRTCChannelProfile profile, const char *channel);
ALIRTCSDK_API int  AliRtcSetClientRole(AliRTCClientRole role, const char *channel);
ALIRTCSDK_API int  AliRtcSetRecordingVolume(int volume, const char *channel);
ALIRTCSDK_API int  AliRtcSetPlayoutVolume(int volume, const char *channel);
ALIRTCSDK_API int  AliRtcEnableAudioVolumeIndication(int interval, int smooth, int reportVad, const char *channel);
ALIRTCSDK_API int  AliRtcSetBeautyEffect(bool enable, const float whiteningLevel,const float smoothnessLevel);
ALIRTCSDK_API int  AliRtcAddVideoWatermark(AliRTCVideoTrack track, const char* imageUrl, bool visibleInPreview, 
  const float Portrait_x, const float Portrait_y, const float Portrait_w, const float Portrait_h,
  const float Landscape_x, const float Landscape_y, const float Landscape_w, const float Landscape_h,
  float alpha, bool normalized);

/*录制接口组 */
ALIRTCSDK_API int AliRtcAddRecordTemplate(const AliRTCRecordTemplate& rTemplate,const AliRTCRecordVideoLayoutMode mode, const AliRTCRecordVideoBgColor& color);
/*添加录制人员 */
ALIRTCSDK_API int AliRtcAddRecordUserStream(const char* uid);
/*移除录制人员 */
ALIRTCSDK_API int AliRtcRemoveRecordUserStream(const char* uid);
/*清空录制人员 */
ALIRTCSDK_API int AliRtcClearAllRecordUserStreams();
/**更新录制内容信息*/
ALIRTCSDK_API bool AliRtcUpdateRecordUserStreams(const char* uid = "");
ALIRTCSDK_API int AliRtcStartRecord(const char* filePath);
ALIRTCSDK_API bool AliRtcPauseRecord();
ALIRTCSDK_API bool AliRtcResumeRecord();
ALIRTCSDK_API bool AliRtcStopRecord();

/* audio raw data*/
ALIRTCSDK_API int  AliRtcSubscribeAudioData(AliRTCAudioSource audioSource, const char *channel);
ALIRTCSDK_API int  AliRtcUnsubscribeAudioData(AliRTCAudioSource audioSource, const char *channel);
ALIRTCSDK_API int  AliRtcSetSubscribeAudioNumChannel(int audioNumChannel, const char *channel);
ALIRTCSDK_API int  AliRtcSetSubscribeAudioSampleRate(int audioSampleRate, const char *channel);
ALIRTCSDK_API int  AliRtcRegisterAudioFrameObserver(const char *channel);
ALIRTCSDK_API int  AliRtcUnRegisterAudioFrameObserver(const char *channel);
ALIRTCSDK_API int  AliRtcSetExternalAudioSource(bool enable, unsigned int sampleRate, unsigned int channelsPerFrame, const char *channel);
ALIRTCSDK_API int  AliRtcPushExternalAudioFrameRawData(const void* audioSamples, unsigned int sampleLength, long long timestamp, const char *channel);
ALIRTCSDK_API int  AliRtcSetExternalAudioPublishVolume(int volume, const char *channel);
ALIRTCSDK_API int  AliRtcGetExternalAudioPublishVolume(const char *channel);
ALIRTCSDK_API int  AliRtcSetMixedWithMic(bool mixed, const char *channel);
ALIRTCSDK_API int  AliRtcSetExteranlAudioRender(bool enable, unsigned int sampleRate, unsigned int channelsPerFrame, const char *channel);
ALIRTCSDK_API int  AliRtcPushExternalAudioRenderRawData(const void* audioSamples, unsigned int sampleLength, unsigned int sampleRate, unsigned int channelsPerFrame, long long timestamp, const char *channel);
ALIRTCSDK_API int  AliRtcSetExternalAudioRenderVolume(int volume, const char *channel);
ALIRTCSDK_API int  AliRtcGetExternalAudioRenderVolume(const char *channel);
/* video raw data */
ALIRTCSDK_API int  AliRtcRegisterVideoSampleObserver(const char *channel);
ALIRTCSDK_API int  AliRtcUnRegisterVideoSampleObserver(const char *channel);
ALIRTCSDK_API int  AliRtcSetExternalVideoSource(bool enable, bool useTexture, AliRTCVideoSource sourceType, int renderMode, const char *channel);
ALIRTCSDK_API int  AliRtcPushExternalVideoFrame(AliRTCVideoDataSample& frame, AliRTCVideoSource sourceType, const char *channel);

#if defined(_WIN32) || defined(__OSX__) //win32 macos only
ALIRTCSDK_API int  AliRtcSetLocalViewConfig(void *displayView, int backgroundColor, int renderMode, int mirrorMode, int scaleMode, int rotation, long long renderId, int width, int height, bool enableGL, AliRTCVideoTrack track);
ALIRTCSDK_API int  AliRtcSetRemoteViewConfig(void *displayView, int backgroundColor, int renderMode, int mirrorMode, int scaleMode, int rotation, long long renderId, int width, int height, bool enableGL, const char* userId, AliRTCVideoTrack track, const char *channel);
ALIRTCSDK_API int  AliRtcStartScreenShareByDesktopId(unsigned int desktopId, bool isShareByRegion, float originX, float originY, float width, float height, const char *channel);
ALIRTCSDK_API int  AliRtcStopScreenShare(const char *channel);
ALIRTCSDK_API const char*  AliRtcGetScreenShareSourceInfo(AliRTCScreenShareType sourceType, const char *channel);
ALIRTCSDK_API bool  AliRtcIsScreenSharePublished(const char *channel);

ALIRTCSDK_API const char*  AliRtcGetAudioCaptures();
ALIRTCSDK_API const char*  AliRtcGetCurrentAudioCaptureName();
ALIRTCSDK_API const char*  AliRtcGetCurrentAudioCaptureId();
ALIRTCSDK_API int  AliRtcSetCurrentAudioCaptureByName(const char* deviceName);
ALIRTCSDK_API int  AliRtcSetCurrentAudioCaptureById(const char* deviceId);
ALIRTCSDK_API const char*  AliRtcGetAudioRenderers();
ALIRTCSDK_API const char*  AliRtcGetCurrentAudioRendererName();
ALIRTCSDK_API const char*  AliRtcGetCurrentAudioRendererId();
ALIRTCSDK_API int  AliRtcSetCurrentAudioRendererByName(const char* deviceName);
ALIRTCSDK_API int  AliRtcSetCurrentAudioRendererById(const char* deviceId);
ALIRTCSDK_API const char*  AliRtcGetCameraList();
ALIRTCSDK_API const char*  AliRtcGetCurrentCameraName();
ALIRTCSDK_API const char*  AliRtcGetCurrentCameraId();
ALIRTCSDK_API int  AliRtcSetCurrentCameraByName(const char* deviceName);
ALIRTCSDK_API int  AliRtcSetCurrentCameraById(const char* deviceId);

ALIRTCSDK_API int  AliRtcStartTestAudioRecordById(const char* deviceId, int callbackInterval = 0);
ALIRTCSDK_API int  AliRtcStartTestAudioRecordByName(const char* deviceName, int callbackInterval = 0);
ALIRTCSDK_API int  AliRtcStopTestAudioRecord();
ALIRTCSDK_API int  AliRtcStartTestAudioPlayoutById(const char* deviceId, const char *wavFilePath, int callbackInterval = 0, int loopCycles = 0);
ALIRTCSDK_API int  AliRtcStartTestAudioPlayoutByName(const char* deviceName, const char *wavFilePath, int callbackInterval = 0, int loopCycles = 0);
ALIRTCSDK_API int  AliRtcStopTestAudioPlayout();

#elif defined(__ANDROID__) || defined(__IOS__) //android iOS only
ALIRTCSDK_API int  AliRtcGenerateTexture();
ALIRTCSDK_API int  AliRtcSetTexture(AliRTCTextureInfo textureInfo, int track, const char *userId);
ALIRTCSDK_API void  AliRtcRemoveTexture(int textureId);
ALIRTCSDK_API int  AliRtcEnableSpeakerphone(bool enable);
ALIRTCSDK_API int  AliRtcSwitchCamera();
ALIRTCSDK_API int  AliRtcSetCameraZoom(float zoom, bool flash, bool autoFocus);
ALIRTCSDK_API bool  AliRtcIsCameraFocusPointSupported();
ALIRTCSDK_API bool  AliRtcIsCameraExposurePointSupported();
ALIRTCSDK_API int  AliRtcSetCameraFocusPoint(float x, float y);
ALIRTCSDK_API int  AliRtcSetCameraExposurePoint(float x, float y);
ALIRTCSDK_API void  AliRtcEnableBackGroundRecording(bool enable);
ALIRTCSDK_API bool  AliRtcIsBackGroundRecording();
#endif

/******************************CallBack********************************/
ALIRTCSDK_API void  AliRtcSetEventOnJoinChannelNotify(ALIRTC_FUNC_OnJoinChannelNotify onJoinChannelNotify);
ALIRTCSDK_API void  AliRtcSetEventOnAudioPublishStateChangedNotify(ALIRTC_FUNC_OnAudioPublishStateChangedNotify onAudioPublishStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnVideoPublishStateChangedNotify(ALIRTC_FUNC_OnVideoPublishStateChangedNotify onVideoPublishStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnDualStreamPublishStateChangedNotify(ALIRTC_FUNC_OnDualStreamPublishStateChangedNotify onDualStreamPublishStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnScreenSharePublishStateChangedNotify(ALIRTC_FUNC_OnScreenSharePublishStateChangedNotify onScreenSharePublishStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnAudioSubscribeStateChangedNotify(ALIRTC_FUNC_OnAudioSubscribeStateChangedNotify onAudioSubscribeStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnVideoSubscribeStateChangedNotify(ALIRTC_FUNC_OnVideoSubscribeStateChangedNotify onVideoSubscribeStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnScreenShareSubscribeStateChangedNotify(ALIRTC_FUNC_OnScreenShareSubscribeStateChangedNotify onScreenShareSubscribeStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnSubscribeStreamTypeChangedNotify(ALIRTC_FUNC_OnSubscribeStreamTypeChangedNotify onSubscribeStreamTypeChangedNotify);

ALIRTCSDK_API void  AliRtcSetEventOnRemoteUserOnLineNotify(ALIRTC_FUNC_OnRemoteUserOnLineNotify onRemoteUserOnLineNotify);
ALIRTCSDK_API void  AliRtcSetEventOnRemoteUserOffLineNotify(ALIRTC_FUNC_OnRemoteUserOffLineNotify onRemoteUserOffLineNotify);
ALIRTCSDK_API void  AliRtcSetEventOnRemoteTrackAvailableNotify(ALIRTC_FUNC_OnRemoteTrackAvailableNotify onRemoteTrackAvailableNotify);
ALIRTCSDK_API void  AliRtcSetEventOnNetworkQualityChangedNotify(ALIRTC_FUNC_OnNetworkQualityChangedNotify onNetworkQualityChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnConnectionLostNotify(ALIRTC_FUNC_OnConnectionLostNotify onConnectionLostNotify);
ALIRTCSDK_API void  AliRtcSetEventOnTryToReconnectNotify(ALIRTC_FUNC_OnTryToReconnectNotify onTryToReconnectNotify);
ALIRTCSDK_API void  AliRtcSetEventOnConnectionRecoveryNotify(ALIRTC_FUNC_OnConnectionRecoveryNotify onConnectionRecoveryNotify);
ALIRTCSDK_API void  AliRtcSetEventOnByeNotify(ALIRTC_FUNC_OnByeNotify onByeNotify);
ALIRTCSDK_API void  AliRtcSetEventOnOccurWarningNotify(ALIRTC_FUNC_OnOccurWarningNotify onOccurWarningNotify);
ALIRTCSDK_API void  AliRtcSetEventOnOccurErrorNotify(ALIRTC_FUNC_OnOccurErrorNotify onOccurErrorNotify);
ALIRTCSDK_API void  AliRtcSetEventOnLeaveChannelResultNotify(ALIRTC_FUNC_OnLeaveChannelResultNotify onLeaveChannelResultNotify);
ALIRTCSDK_API void  AliRtcSetEventOnUpdateRoleNotify(ALIRTC_FUNC_OnUpdateRoleNotify onUpdateRoleNotify);
ALIRTCSDK_API void  AliRtcSetEventOnAudioVolumeCallbackNotify(ALIRTC_FUNC_OnAudioVolumeCallback onAudioVolumeCallbackNotify);
ALIRTCSDK_API void  AliRtcSetEventOnAudioDeviceRecordLevelNotify(ALIRTC_FUNC_OnAudioDeviceRecordLevel onAudioDeviceRecordLevelNotify);
ALIRTCSDK_API void  AliRtcSetEventOnAudioDevicePlayoutLevelNotify(ALIRTC_FUNC_OnAudioDevicePlayoutLevel onAudioDevicePlayoutLevelNotify);
ALIRTCSDK_API void  AliRtcSetEventOnAudioDevicePlayoutEndNotify(ALIRTC_FUNC_OnAudioDevicePlayoutEnd onAudioDevicePlayoutEndNotify);
ALIRTCSDK_API void  AliRtcSetEventOnLastmileDetectResultWithQualityNotify(ALIRTC_FUNC_OnLastmileDetectResultWithQuality onLastmileDetectResultWithQualityNotify);
ALIRTCSDK_API void  AliRtcSetEventOnMediaRecordEventNotify(ALIRTC_FUNC_OnMediaRecordEvent onMediaRecordEventNotify);

ALIRTCSDK_API void  AliRtcSetEventOnRawDataAudioFrameNotify(ALIRTC_FUNC_OnRawDataAudioFrame onRawDataAudioFrameNotify);
ALIRTCSDK_API void  AliRtcSetEventOnPublishAudioFrameNotify(ALIRTC_FUNC_OnPublishAudioFrame onPublishAudioFrameNotify);
ALIRTCSDK_API void  AliRtcSetEventOnSubscribeAudioFrameNotify(ALIRTC_FUNC_OnSubscribeAudioFrame onSubscribeAudioFrameNotify);
ALIRTCSDK_API void  AliRtcSetEventOnPlaybackAudioFrameBeforeMixingNotify(ALIRTC_FUNC_OnPlaybackAudioFrameBeforeMixing onPlaybackAudioFrameBeforeMixingNotify);

ALIRTCSDK_API void  AliRtcSetEventOnCaptureVideoSampleNotify(ALIRTC_FUNC_OnCaptureVideoSample onCaptureVideoSampleNotify);
ALIRTCSDK_API void  AliRtcSetEventOnPreEncodeVideoSampleNotify(ALIRTC_FUNC_OnPreEncodeVideoSample onPreEncodeVideoSampleNotify);
ALIRTCSDK_API void  AliRtcSetEventOnRemoteVideoVideoSampleNotify(ALIRTC_FUNC_OnRemoteVideoVideoSample onRemoteVideoVideoSampleNotify);

#if defined(_WIN32) //win32 only
ALIRTCSDK_API void  AliRtcSetEventOnAudioDeviceStateChangedNotify(ALIRTC_FUNC_OnAudioDeviceStateChangedNotify onAudioDeviceStateChangedNotify);
ALIRTCSDK_API void  AliRtcSetEventOnVideoDeviceStateChangedNotify(ALIRTC_FUNC_OnVideoDeviceStateChangedNotify onVideoDeviceStateChangedNotify);
#endif

#endif /* AliRTCSDKCWrapper_h */
