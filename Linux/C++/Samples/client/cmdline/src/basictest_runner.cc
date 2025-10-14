#include "basictest_runner.h"

#include "cmdline/src/app_config.h"
#include "rtcengine_helper.h"
#include "tlog/tlog.h"
#include "tlog/utils.h"
#include "nlohmann/json.hpp"
#include "external_source_pusher.h"
#include "engine_interface.h"

using namespace tlog;

static uint64_t defaultGetTime()
{
    struct timespec nowTime;
    clock_gettime(CLOCK_REALTIME, &nowTime);
    return (uint64_t) nowTime.tv_sec * 1000 + nowTime.tv_nsec / 1000000;
}
BasicTestRunner::BasicTestRunner(AppOptions* options)
    : TestingRunner(options) {
    TLOGI("BasicTestRunner ctor");
}

BasicTestRunner::~BasicTestRunner() {
    TLOGI("BasicTestRunner dtor");
    Stop();
}

bool BasicTestRunner::Start() {
    TLOGI("BasicTestRunner::Start");
    ResetCompleted();
    // app_config_ = DefaultAppConfigForTest();
    ParseAppConfig(app_options_->app_config_filepath, app_config_);
    join_leave_count_ = app_config_.joinLeaveCount;
    
    std::string cwd = CurrentWorkingDirectory();
    std::string log_dir = GetLogDir();
    TLOGI("set rtcengine log_dir: %s", log_dir.c_str());
    RtcEngine::SetLogLevel(ding::rtc::RtcEngineLogLevelInfo);
    RtcEngine::SetLogDirPath(log_dir.c_str());

    thread_.reset(new Thread());
    thread_->Start();
    thread_->PostTask([this]{
        TestingEntry();
    });
    return true;
}

void BasicTestRunner::Stop() {
    TLOGI("BasicTestRunner::Stop");
    thread_.reset(nullptr);
    LeaveChannel(); // NOTE: need leave channel before destroy?
    DestroyEngine();
}


void BasicTestRunner::TestingEntry() {
    // request auth info
    RtcEngineAuthInfo auth_info;
    bool auth_ok = RequestAuthInfo(app_config_.appServer,
                                   app_config_.appId,
                                   app_config_.userId,
                                   app_config_.userName,
                                   app_config_.channelId,
                                   app_config_.env,
                                   app_config_.passwd,
                                   auth_info);
    // if auth failed, exit
    if (!auth_ok) {
        TLOGE("RequestAuthInfo failed");
        SetCompleted();
        return;
    }
    auth_info_ = auth_info;
    TLOGI("RequestAuthInfo result: ");
    TLOGI("    appId: %s", auth_info.appId.c_str());
    TLOGI("    channelId: %s", auth_info.channelId.c_str());
    TLOGI("    userId: %s", auth_info.userId.c_str());
    TLOGI("    token: %s", auth_info.token.c_str());
    TLOGI("    gslbServer: %s", auth_info.gslbServer.c_str());

    // join channel
    int ret = JoinChannel();
    if (ret != 0) {
        SetCompleted();
    }
}

bool BasicTestRunner::InitEngine() {
    nlohmann::json j_extras;
    j_extras["disableLogToConsole"] = true; // disable sdk log to console
    // if (app_config_.dumpRemoteVideo) {
    //     // TODO: implements with VideoFrameObserver
    //     video_dump_dir_ = cwd + "/video_dump";
    //     TLOGI("video_dump_dir_: %s", video_dump_dir_.c_str());
    //     CreateDirectory(video_dump_dir_.c_str());
    //     j_extras["videoDumpPath"] = video_dump_dir_;
    // }

    std::string extras = j_extras.dump();
    rtc_engine_ = RtcEngine::Create(extras.c_str());
    rtc_engine_->SetEngineEventListener(this);

    if (app_config_.pushExternalAudio || app_config_.pushExternalVideo) {
        external_source_pusher_.reset(new ExternalSourcePusher(rtc_engine_));
    }

    return true;
}

void BasicTestRunner::DestroyEngine() {
    if (rtc_engine_) {
        rtc_engine_->SetEngineEventListener(nullptr);
        RtcEngine::Destroy(rtc_engine_);
        rtc_engine_ = nullptr;
    }
}

int BasicTestRunner::JoinChannel() {
    InitEngine();

    // NOTE: need reset pub/sub before join channel?
    rtc_engine_->PublishLocalAudioStream(app_config_.publishAudio);
    rtc_engine_->PublishLocalVideoStream(app_config_.publishVideo);

    rtc_engine_->SubscribeAllRemoteAudioStreams(app_config_.subscribeAllAudio);
    //rtc_engine_->SubscribeAllRemoteVideoStreams(app_config_.subscribeAllVideo);
    rtc_engine_->SubscribeAllRemoteVideoStreams(app_config_.subscribeAllCameraTracks, app_config_.subscribeAllScreenTracks);
    ding::rtc::RtcEngineVideoStreamType restype;
    if (app_config_.subscribeCameraTrackResolution == "LD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeLD;
    }
    else if (app_config_.subscribeCameraTrackResolution == "SD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeSD;
    }
    else if (app_config_.subscribeCameraTrackResolution == "HD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeHD;
    }
    else if (app_config_.subscribeCameraTrackResolution == "FHD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeFHD;
    }
    rtc_engine_->SetRemoteDefaultVideoStreamType(restype);

    // 订阅视频大小流（所有远端），默认订阅大流
    // RtcEngineVideoStreamType subAllVideoType = RtcEngineVideoStreamTypeHigh; // 视频大小流:
    // RtcEngineVideoStreamTypeHigh, RtcEngineVideoStreamTypeLow
    // engine->SetRemoteDefaultVideoStreamType(subAllVideoType);

    // 测试订阅指定人音视频流
    // OnRemoteUserOnLineNotify 通知远端入会uid
    // OnRemoteTrackAvailableNotify 通知开始接收远端音视频流
    // std::string remote_uid = "gsvegpnuly";

    // 订阅指定人视频
    // bool subVideo = true;
    // RtcEngineVideoTrack videoTrack = RtcEngineVideoTrackCamera; // 视频track类型: RtcEngineVideoTrackCamera,
    // RtcEngineVideoTrackScreen, RtcEngineVideoTrackBoth
    // engine->SubscribeRemoteVideoStream(remote_uid.c_str(), videoTrack, subVideo);

    // RtcEngineVideoStreamType videoStreamType = RtcEngineVideoStreamTypeHigh; // 视频大小流:
    // RtcEngineVideoStreamTypeHigh, RtcEngineVideoStreamTypeLow
    // engine->SetRemoteVideoStreamType(remote_uid.c_str(), videoStreamType);

    RtcEngineVideoEncoderConfiguration video_encoder_config;
    video_encoder_config.dimensions = RtcEngineVideoDimensions(app_config_.videoWidth, app_config_.videoHeight);
    video_encoder_config.frameRate = app_config_.videoFps;
    video_encoder_config.bitrate = app_config_.videoBitrate;
    rtc_engine_->SetVideoEncoderConfiguration(video_encoder_config);

    if (app_config_.dumpPlaybackAudio) {
        audio_dump_dir_ = CurrentWorkingDirectory() + "/audio_dump";
        TLOGI("audio_dump_dir_: %s", audio_dump_dir_.c_str());
        CreateDirectory(audio_dump_dir_.c_str());
        rtc_engine_->RegisterAudioFrameObserver(this);
        rtc_engine_->EnableAudioFrameObserver(true, RtcEngineAudioPositionPlayback);
    }
    if (app_config_.dumpRemoteVideo) {
        video_dump_dir_ = CurrentWorkingDirectory() + "/video_dump";
        TLOGI("video_dump_dir_: %s", video_dump_dir_.c_str());
        CreateDirectory(video_dump_dir_.c_str());
        rtc_engine_->RegisterVideoFrameObserver(this);
        rtc_engine_->EnableVideoFrameObserver(true, RtcEnginePositionPreRender | RtcEnginePositionPreAndSkipDecoder);
    }

    return rtc_engine_->JoinChannel(auth_info_, app_config_.userName.c_str());
}

void BasicTestRunner::LeaveChannel() {
    external_source_pusher_.reset(nullptr);
    if (rtc_engine_) {
        rtc_engine_->LeaveChannel();
    }
}

bool BasicTestRunner::StartPushExternalAudio() {
    RawAudioFileConfig audio_config;
    audio_config.file_path = app_config_.pcmFilePath;
    audio_config.sample_rate = app_config_.pcmSampleRate;
    audio_config.channels = app_config_.pcmChannels;
    audio_config.freq = app_config_.pcmReadFreq;
    if (external_source_pusher_) {
        return external_source_pusher_->StartPushRawAuidoFile(audio_config, app_config_.loopPlayCount);
    }
    return false;
}

bool BasicTestRunner::StartPushExternalVideo() {
    RawVideoFileConfig video_config;
    video_config.file_path = app_config_.videoFilePath;
    video_config.pixel_format = app_config_.videoPixelFormat;
    video_config.width = app_config_.videoWidth;
    video_config.height = app_config_.videoHeight;
    video_config.fps = app_config_.videoFps;
    video_config.rotation = app_config_.videoRotation;
    if (external_source_pusher_) {
        return external_source_pusher_->StartPushRawVideoFile(video_config, app_config_.loopPlayCount);
    }
    return false;
}

void BasicTestRunner::StopPushExternalAudio() {
    if (external_source_pusher_) {
        external_source_pusher_->StopPushRawAuidoFile();
    }
}

void BasicTestRunner::StopPushExternalVideo() {
    if (external_source_pusher_) {
        external_source_pusher_->StopPushRawVideoFile();
    }
}

void BasicTestRunner::CheckPushExternalStatus() {
    if (external_source_pusher_ && (external_source_pusher_->IsRawAudioPushing() || external_source_pusher_->IsRawVideoPushing())) {
        if (thread_) {
            thread_->PostDelayedTask([this] { CheckPushExternalStatus(); }, 1000);
        }
    } else {
        TLOGI("external source push finished, then leave channel");
        // finally leave channel
        LeaveChannel();
    }
}

// --- implements RtcEngineEventListener ---
void BasicTestRunner::OnJoinChannelResult(int result, const char *channel, const char *userId, int elapsed) {
    TLOGI("OnJoinChannelResult: result: %d, channel: %s, userId: %s, elapsed: %d", result, channel, userId, elapsed);
    if (!thread_) {
        return;
    }
    // start push external source until finished, or leave channel directly
    thread_->PostTask([this, result]{
        if (result != 0) {
            SetCompleted();
            return;
        }
        if (!(app_config_.pushExternalAudio || app_config_.pushExternalVideo)) {
            LeaveChannel();
            return;
        }
        if (app_config_.pushExternalAudio) {
            StartPushExternalAudio();
        }
        if (app_config_.pushExternalVideo) {
            StartPushExternalVideo();
        }
        CheckPushExternalStatus();
    });
}

void BasicTestRunner::OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats) {
    TLOGI("OnLeaveChannelResult: result: %d", result);
    if (!thread_) {
        return;
    }
    thread_->PostDelayedTask([this, result]{
        TLOGI("join_leave_count_: %d", join_leave_count_);
        if (join_leave_count_ > 0) {
            --join_leave_count_;
        }
        if (join_leave_count_ == 0) {
            SetCompleted();
        } else {
            DestroyEngine();
            JoinChannel();
        }
    }, 1000);
}

void BasicTestRunner::OnRemoteUserOnLineNotify(const char *uid, int elapsed) {
    TLOGI("OnRemoteUserOnLineNotify, uid: %s, elapsed: %d", uid, elapsed);
}
void BasicTestRunner::OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason) {
    TLOGI("OnRemoteUserOffLineNotify, uid: %d, reason: %d", uid, reason);
}
void BasicTestRunner::OnBye(ding::rtc::RtcEngineOnByeType code) {
    TLOGI("OnBye: code: %d", code);
}
void BasicTestRunner::OnConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status,
                                                  ding::rtc::RtcEngineConnectionStatusChangeReason reason) {
    TLOGI("OnConnectionStatusChange: status: %d, reason: %d", status, reason);
}
void BasicTestRunner::OnNetworkQualityChanged(const char *uid,
                                                ding::rtc::RtcEngineNetworkQuality upQuality,
                                                ding::rtc::RtcEngineNetworkQuality downQuality) {
    TLOGI("OnNetworkQualityChanged: uid: %s, up: %d, down: %d", uid, upQuality, downQuality);
}
void BasicTestRunner::OnStatisticsLog(ding::rtc::String const &log) {
    // TLOGI("OnStatisticsLog: log: %s", log.c_str());
}
void BasicTestRunner::OnOccurWarning(int warn, const char *msg) {
    TLOGI("OnOccurWarning: warn: %d, msg: %s", warn, msg);
}
void BasicTestRunner::OnOccurError(int error, const char *msg) {
    TLOGI("OnOccurError: error: %d, msg: %s", error, msg);
}
void BasicTestRunner::OnStatisticsLevelLog(int level, const ding::rtc::String &log) {
    // TLOGI("OnStatisticsLevelLog: level: %d, log: %s", level, log.c_str());
}
void BasicTestRunner::OnQualityReport(const ding::rtc::QualityReport &quality) {
    TLOGI("OnQualityReport: quality: %d", quality);
}
void BasicTestRunner::OnLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats) {
    // TLOGI("OnLocalAudioStats, stats: %s", LocalAudioStatsToString(localAudioStats).c_str());
}
void BasicTestRunner::OnLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats) {
    // TLOGI("OnLocalVideoStats, stats: %s", LocalVideoStatsToString(localVideoStats).c_str());
}
void BasicTestRunner::OnRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats) {
    // TLOGI("OnRemoteAudioStats, stats: %s", RemoteAudioStatsToString(remoteAudioStats).c_str());
}
void BasicTestRunner::OnRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats) {
    // TLOGI("OnRemoteVideoStats, stats: %s", RemoteVideoStatsToString(remoteVideoStats).c_str());
}
void BasicTestRunner::OnStats(const ding::rtc::RtcEngineStats &stats) {
    // TLOGI("OnStats, stats: %s", RtcEngineStatsToString(stats).c_str());
}
void BasicTestRunner::OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                   ding::rtc::RtcEnginePublishState newState,
                                                   int elapseSinceLastState,
                                                   const char *channel) {
    TLOGI("OnAudioPublishStateChanged: oldstate: %d, newstate: %d, channel: %s", oldState, newState, channel);
}
void BasicTestRunner::OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                   ding::rtc::RtcEnginePublishState newState,
                                                   int elapseSinceLastState,
                                                   const char *channel) {
    TLOGI("OnVideoPublishStateChanged: oldstate: %d, newstate: %d, elapsed: %d, channel: %s",
          oldState,
          newState,
          elapseSinceLastState,
          channel);
}
void BasicTestRunner::OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                         ding::rtc::RtcEnginePublishState newState,
                                                         int elapseSinceLastState,
                                                         const char *channel) {
    TLOGI("OnScreenSharePublishStateChanged: oldstate: %d, newstate: %d, elapsed: %d, channel: %s",
          oldState,
          newState,
          elapseSinceLastState,
          channel);
}
void BasicTestRunner::OnRemoteTrackAvailableNotify(const char *uid,
                                                     ding::rtc::RtcEngineAudioTrack audioTrack,
                                                     ding::rtc::RtcEngineVideoTrack videoTrack) {
    TLOGI("OnRemoteTrackAvailableNotify: uid: %s, audioTrack: %d, videoTrack: %d", uid, audioTrack, videoTrack);
}
void BasicTestRunner::OnUserAudioMuted(const char *uid, bool isMute) {
    TLOGI("OnUserAudioMuted: uid: %s, isMute: %d", uid, isMute);
}
void BasicTestRunner::OnUserVideoMuted(const char *uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack) {
    TLOGI("OnUserVideoMuted: uid: %s, isMute: %d, videoTrack: %d", uid, isMute, videoTrack);
}
void BasicTestRunner::OnFirstRemoteVideoFrameDrawn(const char *uid,
                                                     ding::rtc::RtcEngineVideoTrack videoTrack,
                                                     int width,
                                                     int height,
                                                     int elapsed) {
    TLOGI("OnFirstRemoteVideoFrameDrawn: uid: %s, videoTrack: %d, width: %d, height: %d, elapsed: %d",
          uid, videoTrack, width, height, elapsed);
}
void BasicTestRunner::OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed) {
    TLOGI("OnFirstLocalVideoFrameDrawn:width: %d, height: %d, elapsed: %d", width, height, elapsed);
}
void BasicTestRunner::OnFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) {
    TLOGI("OnFirstVideoPacketSend, ");
}
void BasicTestRunner::OnFirstVideoPacketReceived(const char *uid,
                                                   ding::rtc::RtcEngineVideoTrack videoTrack,
                                                   int timeCost) {
    TLOGI("OnFirstVideoPacketReceived, ");
}
void BasicTestRunner::OnFirstVideoFrameReceived(const char *uid,
                                                  ding::rtc::RtcEngineVideoTrack videoTrack,
                                                  int timeCost) {
    TLOGI("OnFirstVideoFrameReceived, ");
}
void BasicTestRunner::OnFirstAudioPacketSend(int timeCost) {
    TLOGI("OnFirstAudioPacketSend, ");
}
void BasicTestRunner::OnFirstAudioPacketReceived(const char *uid, int timeCost) {
    TLOGI("OnFirstAudioPacketReceived, ");
}
void BasicTestRunner::OnAudioSubscribeStateChanged(const char *uid,
                                                     ding::rtc::RtcEngineSubscribeState oldState,
                                                     ding::rtc::RtcEngineSubscribeState newState,
                                                     int elapseSinceLastState,
                                                     const char *channel) {
    TLOGI("OnAudioSubscribeStateChanged: uid: %s, oldstate: %d, newstate: %d, elapsed: %d, channel: %s",
          uid,
          oldState,
          newState,
          elapseSinceLastState,
          channel);
}
void BasicTestRunner::OnVideoSubscribeStateChanged(const char *uid,
                                                     ding::rtc::RtcEngineSubscribeState oldState,
                                                     ding::rtc::RtcEngineSubscribeState newState,
                                                     int elapseSinceLastState,
                                                     const char *channel) {
    TLOGI("OnVideoSubscribeStateChanged: uid: %s, oldstate: %d, newstate: %d, elapsed: %d, channel: %s",
          uid,
          oldState,
          newState,
          elapseSinceLastState,
          channel);
}
void BasicTestRunner::OnScreenShareSubscribeStateChanged(const char *uid,
                                                           ding::rtc::RtcEngineSubscribeState oldState,
                                                           ding::rtc::RtcEngineSubscribeState newState,
                                                           int elapseSinceLastState,
                                                           const char *channel) {
    TLOGI("OnScreenShareSubscribeStateChanged: uid: %s, oldstate: %d, newstate: %d, elapsed: %d, channel: %s",
          uid,
          oldState,
          newState,
          elapseSinceLastState,
          channel);
}
void BasicTestRunner::OnSubscribeStreamTypeChanged(const char *uid,
                                                     ding::rtc::RtcEngineVideoStreamType oldStreamType,
                                                     ding::rtc::RtcEngineVideoStreamType newStreamType,
                                                     int elapseSinceLastState,
                                                     const char *channel) {
    TLOGI("OnSubscribeStreamTypeChanged: uid: %s, old: %d, new: %d, elapsed: %d, channel: %s",
          uid,
          oldStreamType,
          newStreamType,
          elapseSinceLastState,
          channel);
}
void BasicTestRunner::OnRecordingDeviceAudioLevel(int level) {
    TLOGI("OnRecordingDeviceAudioLevel: level: %d", level);
}
void BasicTestRunner::OnPlayoutDeviceAudioLevel(int level) {
    TLOGI("OnPlayoutDeviceAudioLevel: level: %d", level);
}
void BasicTestRunner::OnAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers,
                                                unsigned int speakerNumber) {
    TLOGI("OnAudioVolumeIndication, ");
}
void BasicTestRunner::OnAudioDeviceStateChanged(const char *deviceId,
                                                  ding::rtc::RtcEngineDeviceType deviceType,
                                                  ding::rtc::RtcEngineDeviceState deviceState) {
    TLOGI("OnAudioDeviceStateChanged: deviceId: %s, deviceType: %d, deviceState: %d", deviceId, deviceType, deviceState);
}
void BasicTestRunner::OnApiCalledExecuted(int error, const char *api, const char *result) {
    TLOGI("OnApiCalledExecuted, error: %d, api: %s, result: %s", error, api?api:"", result?result:"");
}
void BasicTestRunner::OnSnapshotComplete(const char *userId,
                                           ding::rtc::RtcEngineVideoTrack videoTrack,
                                           const char *filePath,
                                           int width,
                                           int height,
                                           bool success) {
    TLOGI("OnSnapshotComplete, ");
}

void BasicTestRunner::OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) {
    uint64_t now_time = defaultGetTime();
    printf("OnPlaybackAudioFrame %lld\n", now_time);
    if (!playback_audio_dump_file_) {
        char filename[1024] = {0};
        snprintf(filename, sizeof(filename), "%s/audio_playback_%d_%d_%s.pcm", audio_dump_dir_.c_str(),
            frame.samplesPerSec, frame.channels, DateTimeFormat().c_str());
        playback_audio_dump_file_ = fopen(filename, "wb+");
        TLOGI("playback audio dump file: %s", filename);
    }

    if (playback_audio_dump_file_) {
        int bytesToWrite = frame.samples * frame.channels * frame.bytesPerSample;
        if (bytesToWrite > 0) {
            fwrite(frame.buffer, 1, bytesToWrite, playback_audio_dump_file_);
        }
    }
}

RtcEngineVideoPixelFormat BasicTestRunner::GetVideoFormatPreference() {
    return RtcEngineVideoI420;
}

bool BasicTestRunner::OnCaptureVideoFrame(ding::rtc::RtcEngineVideoFrame &frame) {
    return true;
}

bool BasicTestRunner::OnRemoteVideoFrame(ding::rtc::String uid, ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) {
    if (!remote_video_dump_file_) {
        char filename[1024] = {0};
        snprintf(filename, sizeof(filename), "%s/remote_video_%s_%d_%d_%s.yuv", video_dump_dir_.c_str(),
            uid.c_str(), frame.width, frame.height, DateTimeFormat().c_str());
        remote_video_dump_file_ = fopen(filename, "wb+");
        TLOGI("remote video dump file: %s", filename);
    }

    if (remote_video_dump_file_) {
        int bytesToWrite = frame.width * frame.height * 3 / 2;
        if (bytesToWrite > 0) {
            fwrite(frame.data, 1, bytesToWrite, remote_video_dump_file_);
        }
    }
    return true;
}

bool BasicTestRunner::OnPreEncodeVideoFrame(ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) {
    return true;
}

void BasicTestRunner::OnPreDecodeVideoFrame(
        ding::rtc::String uid,
        ding::rtc::RtcEngineVideoTrack track,
        ding::rtc::RtcEngineEncodedVideoFrame &frame)
{
    uint64_t now_time = defaultGetTime();
    printf("tm %lld got h264 frame: uid %s track %d frame format %d length %d rotation %d mirror %s\n",
        now_time, uid.c_str(), track, frame.format, frame.length, frame.rotation, frame.mirror ? "true" : "false");
}
