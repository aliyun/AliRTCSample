#include "capi_test_runner.h"

#include <inttypes.h>
#include <string.h>

#include "tlog/tlog.h"
#include "tlog/utils.h"
#include "nlohmann/json.hpp"
#include "rtcengine_helper.h"
#include "external_source_pusher_c.h"

static const int kBytePerSamplePcm16 = 2;
static const int kStatsIntervalMs = 5000;

CApiTestRunner::CApiTestRunner(AppOptions* options)
    : TestingRunner(options) {
    TLOGI("CApiTestRunner ctor");
}

CApiTestRunner::~CApiTestRunner() {
    TLOGI("CApiTestRunner dtor");
    Stop();
}

bool CApiTestRunner::Start() {
    TLOGI("CApiTestRunner::Start");
    ResetCompleted();
    ParseAppConfig(app_options_->app_config_filepath, app_config_);
    join_leave_count_ = app_config_.joinLeaveCount;
    InitEngine();

    DingRtcDeviceInfo *recordingDevices = nullptr;
    DingRtcDeviceInfo *playoutDevices = nullptr;
    int recordingDeviceCount = 0;
    int playoutDeviceCount = 0;
    DingRtc_GetRecordingDeviceList(rtc_handle_, &recordingDevices, &recordingDeviceCount);
    DingRtc_GetPlayoutDeviceList(rtc_handle_, &playoutDevices, &playoutDeviceCount);
    TLOGI("recordingDeviceCount: %d, playoutDeviceCount: %d", recordingDeviceCount, playoutDeviceCount);
    for (int i = 0; i < recordingDeviceCount; i++) {
        TLOGI("recordingDevice[%d], deviceId: %s, deviceName: %s", i, recordingDevices[i].deviceId, recordingDevices[i].deviceName);
    }
    for (int i = 0; i < playoutDeviceCount; i++) {
        TLOGI("playoutDevice[%d], deviceId: %s, deviceName: %s", i, playoutDevices[i].deviceId, playoutDevices[i].deviceName);
    }
    DingRtc_DeviceInfo_Free(&recordingDevices, recordingDeviceCount);
    DingRtc_DeviceInfo_Free(&playoutDevices, playoutDeviceCount);

    thread_.reset(new tlog::Thread());
    thread_->Start();
    thread_->PostTask([this]{
        TestingEntry();
    });
    return true;
}

void CApiTestRunner::Stop() {
    TLOGI("CApiTestRunner::Stop");
    thread_.reset(nullptr);
    LeaveChannel();
    DestroyEngine();

    if (playback_audio_dump_file_) {
        fclose(playback_audio_dump_file_);
        playback_audio_dump_file_ = nullptr;
    }
    if (remote_video_dump_file_) {
        fclose(remote_video_dump_file_);
        remote_video_dump_file_ = nullptr;
    }
    for(auto& e : remote_user_audio_files_) {
        fclose(e.second);
    }
    remote_user_audio_files_.clear();
}

void CApiTestRunner::TestingEntry() {
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
    TLOGI("RequestAuthInfo result: ");
    TLOGI("    appId: %s", auth_info.appId.c_str());
    TLOGI("    channelId: %s", auth_info.channelId.c_str());
    TLOGI("    userId: %s", auth_info.userId.c_str());
    TLOGI("    token: %s", auth_info.token.c_str());
    TLOGI("    gslbServer: %s", auth_info.gslbServer.c_str());

    auth_info_.appId = strdup(auth_info.appId.c_str());
    auth_info_.channelId = strdup(auth_info.channelId.c_str());
    auth_info_.userId = strdup(auth_info.userId.c_str());
    auth_info_.token = strdup(auth_info.token.c_str());
    auth_info_.gslbServer = strdup(auth_info.gslbServer.c_str());

    // join channel
    int ret = JoinChannel();
    if (ret != 0) {
        SetCompleted();
    }
}

bool CApiTestRunner::InitEngine() {
    std::string cwd = tlog::CurrentWorkingDirectory();
    std::string log_dir = GetLogDir();
    TLOGI("set rtcengine log_dir: %s", log_dir.c_str());
    DingRtc_SetLogLevel(DingRtcLogLevelInfo);
    DingRtc_SetLogDirPath(log_dir.c_str());

    nlohmann::json j_extras;
    j_extras["disableLogToConsole"] = true; // disable sdk log to console
    if (app_config_.dumpRemoteVideo) {
        // TODO: implements with VideoFrameObserver
        video_dump_dir_ = cwd + "/video_dump";
        TLOGI("video_dump_dir_: %s", video_dump_dir_.c_str());
        tlog::CreateDirectory(video_dump_dir_.c_str());
        j_extras["videoDumpPath"] = video_dump_dir_;
    }

    std::string extras = j_extras.dump();
    rtc_handle_ = DingRtc_Create(extras.c_str());

    event_handler_.onJoinChannelResult = &CApiTestRunner::OnJoinChannelResult;
    event_handler_.onLeaveChannelResult = &CApiTestRunner::OnLeaveChannelResult;
    event_handler_.onRemoteUserOnLineNotify = &CApiTestRunner::OnRemoteUserOnLineNotify;
    event_handler_.onRemoteUserOffLineNotify = &CApiTestRunner::OnRemoteUserOffLineNotify;
    event_handler_.onBye = &CApiTestRunner::OnBye;
    event_handler_.onConnectionStatusChanged = &CApiTestRunner::OnConnectionStatusChanged;
    event_handler_.onNetworkQualityChanged = &CApiTestRunner::OnNetworkQualityChanged;
    event_handler_.onStatisticsLog = &CApiTestRunner::OnStatisticsLog;
    event_handler_.onOccurWarning = &CApiTestRunner::OnOccurWarning;
    event_handler_.onOccurError = &CApiTestRunner::OnOccurError;
    event_handler_.onStatisticsLevelLog = &CApiTestRunner::OnStatisticsLevelLog;
    event_handler_.onQualityReport = &CApiTestRunner::OnQualityReport;
    event_handler_.onLocalAudioStats = &CApiTestRunner::OnLocalAudioStats;
    event_handler_.onLocalVideoStats = &CApiTestRunner::OnLocalVideoStats;
    event_handler_.onRemoteAudioStats = &CApiTestRunner::OnRemoteAudioStats;
    event_handler_.onRemoteVideoStats = &CApiTestRunner::OnRemoteVideoStats;
    event_handler_.onStats = &CApiTestRunner::OnStats;
    event_handler_.onAudioPublishStateChanged = &CApiTestRunner::OnAudioPublishStateChanged;
    event_handler_.onVideoPublishStateChanged = &CApiTestRunner::OnVideoPublishStateChanged;
    event_handler_.onScreenSharePublishStateChanged = &CApiTestRunner::OnScreenSharePublishStateChanged;
    event_handler_.onRemoteTrackAvailableNotify = &CApiTestRunner::OnRemoteTrackAvailableNotify;
    event_handler_.onUserAudioMuted = &CApiTestRunner::OnUserAudioMuted;
    event_handler_.onUserVideoMuted = &CApiTestRunner::OnUserVideoMuted;
    event_handler_.onFirstRemoteVideoFrameDrawn = &CApiTestRunner::OnFirstRemoteVideoFrameDrawn;
    event_handler_.onFirstLocalVideoFrameDrawn = &CApiTestRunner::OnFirstLocalVideoFrameDrawn;
    event_handler_.onFirstVideoPacketSend = &CApiTestRunner::OnFirstVideoPacketSend;
    event_handler_.onFirstVideoPacketReceived = &CApiTestRunner::OnFirstVideoPacketReceived;
    event_handler_.onFirstVideoFrameReceived = &CApiTestRunner::OnFirstVideoFrameReceived;
    event_handler_.onFirstAudioPacketSend = &CApiTestRunner::OnFirstAudioPacketSend;
    event_handler_.onFirstAudioPacketReceived = &CApiTestRunner::OnFirstAudioPacketReceived;
    event_handler_.onAudioSubscribeStateChanged = &CApiTestRunner::OnAudioSubscribeStateChanged;
    event_handler_.onVideoSubscribeStateChanged = &CApiTestRunner::OnVideoSubscribeStateChanged;
    event_handler_.onScreenShareSubscribeStateChanged = &CApiTestRunner::OnScreenShareSubscribeStateChanged;
    event_handler_.onSubscribeStreamTypeChanged = &CApiTestRunner::OnSubscribeStreamTypeChanged;
    event_handler_.onRecordingDeviceAudioLevel = &CApiTestRunner::OnRecordingDeviceAudioLevel;
    event_handler_.onPlayoutDeviceAudioLevel = &CApiTestRunner::OnPlayoutDeviceAudioLevel;
    event_handler_.onAudioVolumeIndication = &CApiTestRunner::OnAudioVolumeIndication;
    event_handler_.onAudioDeviceStateChanged = &CApiTestRunner::OnAudioDeviceStateChanged;
    event_handler_.onApiCalledExecuted = &CApiTestRunner::OnApiCalledExecuted;
    event_handler_.onSnapshotComplete = &CApiTestRunner::OnSnapshotComplete;
    event_handler_.userData = this;
    DingRtc_SetEventHandler(rtc_handle_, &event_handler_);

    // if (app_config_.dumpRemoteVideo) {
    //     videoFrameObserver = new RtcVideoFrameObserver();
    //     rtc_engine_->RegisterVideoFrameObserver(ctx->videoFrameObserver);
    // }
    if (app_config_.pushExternalAudio || app_config_.pushExternalVideo) {
        external_source_pusher_.reset(new ExternalSourcePusherC(rtc_handle_));
    }

    return true;
}

void CApiTestRunner::DestroyEngine() {
    DingRtc_SetEventHandler(rtc_handle_, nullptr);
    DingRtc_Destroy(rtc_handle_);
    rtc_handle_ = nullptr;
}

int CApiTestRunner::JoinChannel() {
    DingRtc_PublishLocalAudioStream(rtc_handle_, app_config_.publishAudio);
    DingRtc_PublishLocalVideoStream(rtc_handle_, app_config_.publishVideo);

    DingRtc_SubscribeAllRemoteAudioStreams(rtc_handle_, app_config_.subscribeAllAudio);
    DingRtc_SubscribeAllRemoteVideoStreams(rtc_handle_, app_config_.subscribeAllCameraTracks && app_config_.subscribeAllScreenTracks);

    DingRtcVideoEncoderConfiguration video_encoder_config;
    video_encoder_config.dimensions.width = app_config_.videoWidth;
    video_encoder_config.dimensions.height = app_config_.videoHeight;
    video_encoder_config.frameRate = app_config_.videoFps;
    DingRtc_SetVideoEncoderConfiguration(rtc_handle_, video_encoder_config);

    if (app_config_.dumpPlaybackAudio) {
        audio_dump_dir_ = tlog::CurrentWorkingDirectory() + "/audio_dump";
        TLOGI("audio_dump_dir_: %s", audio_dump_dir_.c_str());
        tlog::CreateDirectory(audio_dump_dir_.c_str());

        audio_frame_observer_.onPlaybackAudioFrame = &CApiTestRunner::OnPlaybackAudioFrame;
        audio_frame_observer_.onCapturedAudioFrame = &CApiTestRunner::OnCapturedAudioFrame;
        audio_frame_observer_.onProcessCapturedAudioFrame = &CApiTestRunner::OnProcessCapturedAudioFrame;
        audio_frame_observer_.onPublishAudioFrame = &CApiTestRunner::OnPublishAudioFrame;
        audio_frame_observer_.onRemoteUserAudioFrame = &CApiTestRunner::OnRemoteUserAudioFrame;
        audio_frame_observer_.userData = this;
        DingRtc_RegisterAudioFrameObserver(rtc_handle_, &audio_frame_observer_);
        struct DingRtcAudioFrameObserverConfig config {
            .sampleRate = DingRtcAudioSampleRate_16000,
            .channels = DingRtcMonoAudio,
            .permission = DingRtcAudioFrameReadOnly,
        };
        DingRtc_EnableAudioFrameObserverWithConfig(rtc_handle_, true, DingRtcAudioPositionRemoteUser, config);
        DingRtc_EnableAudioFrameObserverWithConfig(rtc_handle_, true, DingRtcAudioPositionPlayback, config);
    }

    return DingRtc_JoinChannel(rtc_handle_, auth_info_, app_config_.userName.c_str());
}

void CApiTestRunner::LeaveChannel() {
    external_source_pusher_.reset(nullptr);
    DingRtc_LeaveChannel(rtc_handle_);

    if (join_leave_count_ > 0) {
        --join_leave_count_;
    }
    if (join_leave_count_ == 0) {
        SetCompleted();
    }
}

bool CApiTestRunner::StartPushExternalAudio() {
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

bool CApiTestRunner::StartPushExternalVideo() {
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

void CApiTestRunner::StopPushExternalAudio() {
    if (external_source_pusher_) {
        external_source_pusher_->StopPushRawAuidoFile();
    }
}

void CApiTestRunner::StopPushExternalVideo() {
    if (external_source_pusher_) {
        external_source_pusher_->StopPushRawVideoFile();
    }
}

void CApiTestRunner::CheckPushExternalStatus() {
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

void CApiTestRunner::OnJoinChannelResult(DingRtcEventHandler *handler, int result, const char* channel, const char* userId, int elapsed) {
    TLOGI("OnJoinChannelResult, result: %d, channel: %s, userId: %s, elapsed: %d", result, channel, userId, elapsed);
    CApiTestRunner *p_this = (CApiTestRunner *)handler->userData;
    if (p_this) {
        p_this->OnJoinChannelResult(result, channel, userId, elapsed);
    }
}

void CApiTestRunner::OnLeaveChannelResult(DingRtcEventHandler *handler, int result, const DingRtcStats* stats) {
    TLOGI("OnLeaveChannelResult, result: %d", result);
    CApiTestRunner *p_this = (CApiTestRunner *)handler->userData;
    if (p_this) {
        p_this->OnLeaveChannelResult(result, stats);
    }
}

void CApiTestRunner::OnRemoteUserOnLineNotify(DingRtcEventHandler *handler, const char* uid, int elapsed) {
    TLOGI("OnRemoteUserOnLineNotify, uid: %s, elapsed: %d", uid, elapsed);
    CApiTestRunner *p_this = (CApiTestRunner *)handler->userData;
    if (p_this) {
        p_this->OnRemoteUserOnLineNotify(uid, elapsed);
    }
}

void CApiTestRunner::OnRemoteUserOffLineNotify(DingRtcEventHandler *handler, const char* uid, DingRtcUserOfflineReason reason) {
    TLOGI("OnRemoteUserOffLineNotify, uid: %d, reason: %d", uid, reason);
    CApiTestRunner *p_this = (CApiTestRunner *)handler->userData;
    if (p_this) {
        p_this->OnRemoteUserOffLineNotify(uid, reason);
    }
}

void CApiTestRunner::OnBye(DingRtcEventHandler *handler, DingRtcOnByeType code) {
    TLOGI("OnBye, code: %d", code);
}

void CApiTestRunner::OnConnectionStatusChanged(DingRtcEventHandler *handler, DingRtcConnectionStatus status, DingRtcConnectionStatusChangeReason reason) {
    TLOGI("OnConnectionStatusChange, status: %d, reason: %d", status, reason);
}

void CApiTestRunner::OnNetworkQualityChanged(DingRtcEventHandler *handler, const char* uid, DingRtcNetworkQuality upQuality,DingRtcNetworkQuality downQuality) {
    TLOGI("OnNetworkQualityChanged, uid: %s, up: %d, down: %d", uid, upQuality, downQuality);
}

void CApiTestRunner::OnStatisticsLog(DingRtcEventHandler *handler, const char* log) {
    TLOGI("OnStatisticsLog, log: %s", log);
}

void CApiTestRunner::OnOccurWarning(DingRtcEventHandler *handler, int warn, const char* msg) {
    TLOGI("OnOccurWarning, warn: %d, msg: %s", warn, msg);
}

void CApiTestRunner::OnOccurError(DingRtcEventHandler *handler, int error, const char* msg) {
    TLOGI("OnOccurError, error: %d, msg: %s", error, msg);
}

void CApiTestRunner::OnStatisticsLevelLog(DingRtcEventHandler *handler, int level, const char* log) {
    TLOGI("OnStatisticsLevelLog, level: %d, log: %s", level, log);
}

void CApiTestRunner::OnQualityReport(DingRtcEventHandler *handler, const DingRtcQualityReport* quality) {
    TLOGI("OnQualityReport, quality: %d", quality);
}

void CApiTestRunner::OnLocalAudioStats(DingRtcEventHandler *handler, const DingRtcLocalAudioStats* localAudioStats) {
    TLOGI("OnLocalAudioStats");
}

void CApiTestRunner::OnLocalVideoStats(DingRtcEventHandler *handler, const DingRtcLocalVideoStats* localVideoStats) {
    TLOGI("OnLocalVideoStats");
}

void CApiTestRunner::OnRemoteAudioStats(DingRtcEventHandler *handler, const DingRtcRemoteAudioStats* remoteAudioStats) {
    TLOGI("OnRemoteAudioStats");
}

void CApiTestRunner::OnRemoteVideoStats(DingRtcEventHandler *handler, const DingRtcRemoteVideoStats* remoteVideoStats) {
    TLOGI("OnRemoteVideoStats");
}

void CApiTestRunner::OnStats(DingRtcEventHandler *handler, const DingRtcStats* stats) {
    TLOGI("OnStats");
}

void CApiTestRunner::OnAudioPublishStateChanged(DingRtcEventHandler *handler, DingRtcPublishState oldState, DingRtcPublishState newState, int elapseSinceLastState, const char* channel) {
    TLOGI("OnAudioPublishStateChanged, oldstate: %d, newstate: %d, channel: %s", oldState, newState, channel);
}

void CApiTestRunner::OnVideoPublishStateChanged(DingRtcEventHandler *handler, DingRtcPublishState oldState, DingRtcPublishState newState, int elapseSinceLastState, const char* channel) {
    TLOGI("OnVideoPublishStateChanged, oldstate: %d, newstate: %d, elapsed: %d, channel: %s", oldState, newState, elapseSinceLastState, channel);
}

void CApiTestRunner::OnScreenSharePublishStateChanged(DingRtcEventHandler *handler, DingRtcPublishState oldState, DingRtcPublishState newState, int elapseSinceLastState, const char* channel) {
    TLOGI("OnScreenSharePublishStateChanged, oldstate: %d, newstate: %d, elapsed: %d, channel: %s", oldState, newState, elapseSinceLastState, channel);
}

void CApiTestRunner::OnRemoteTrackAvailableNotify(DingRtcEventHandler *handler, const char* uid, DingRtcAudioTrack audioTrack, DingRtcVideoTrack videoTrack) {
    TLOGI("OnRemoteTrackAvailableNotify, uid: %s, audioTrack: %d, videoTrack: %d", uid, audioTrack, videoTrack);
}

void CApiTestRunner::OnUserAudioMuted(DingRtcEventHandler *handler, const char* uid, bool isMute) {
    TLOGI("OnUserAudioMuted, uid: %s, isMute: %d", uid, isMute);
}

void CApiTestRunner::OnUserVideoMuted(DingRtcEventHandler *handler, const char* uid, bool isMute, DingRtcVideoTrack videoTrack) {
    TLOGI("OnUserVideoMuted, uid: %s, isMute: %d, videoTrack: %d", uid, isMute, videoTrack);
}

void CApiTestRunner::OnFirstRemoteVideoFrameDrawn(DingRtcEventHandler *handler, const char* uid, DingRtcVideoTrack videoTrack, int width, int height, int elapsed) {
    TLOGI("OnFirstRemoteVideoFrameDrawn, uid: %s, videoTrack: %d, width: %d, height: %d, elapsed: %d",
          uid, videoTrack, width, height, elapsed);
}

void CApiTestRunner::OnFirstLocalVideoFrameDrawn(DingRtcEventHandler *handler, int width, int height, int elapsed) {
    TLOGI("OnFirstLocalVideoFrameDrawn, width: %d, height: %d, elapsed: %d", width, height, elapsed);
}

void CApiTestRunner::OnFirstVideoPacketSend(DingRtcEventHandler *handler, DingRtcVideoTrack videoTrack, int timeCost) {
    TLOGI("OnFirstVideoPacketSend, videoTrack: %d, timeCost: %d", videoTrack, timeCost);
}

void CApiTestRunner::OnFirstVideoPacketReceived(DingRtcEventHandler *handler, const char* uid, DingRtcVideoTrack videoTrack, int timeCost) {
    TLOGI("OnFirstVideoPacketReceived, uid: %s, videoTrack: %d, timeCost: %d", uid, videoTrack, timeCost);
}

void CApiTestRunner::OnFirstVideoFrameReceived(DingRtcEventHandler *handler, const char* uid, DingRtcVideoTrack videoTrack, int timeCost) {
    TLOGI("OnFirstVideoFrameReceived, uid: %s, videoTrack: %d, timeCost: %d", uid, videoTrack, timeCost);
}

void CApiTestRunner::OnFirstAudioPacketSend(DingRtcEventHandler *handler, int timeCost) {
    TLOGI("OnFirstAudioPacketSend, timeCost: %d", timeCost);
}

void CApiTestRunner::OnFirstAudioPacketReceived(DingRtcEventHandler *handler, const char* uid, int timeCost) {
    TLOGI("OnFirstAudioPacketReceived, uid: %s, timeCost: %d", uid, timeCost);
}

void CApiTestRunner::OnAudioSubscribeStateChanged(DingRtcEventHandler *handler, const char* uid, DingRtcSubscribeState oldState, DingRtcSubscribeState newState, int elapseSinceLastState, const char* channel) {
    TLOGI("OnAudioSubscribeStateChanged, uid: %s, oldstate: %d, newstate: %d, elapseSinceLastState: %d, channel: %s", uid, oldState, newState, elapseSinceLastState, channel);
}

void CApiTestRunner::OnVideoSubscribeStateChanged(DingRtcEventHandler *handler, const char* uid, DingRtcSubscribeState oldState, DingRtcSubscribeState newState, int elapseSinceLastState, const char* channel) {
    TLOGI("OnVideoSubscribeStateChanged, uid: %s, oldstate: %d, newstate: %d, elapseSinceLastState: %d, channel: %s", uid, oldState, newState, elapseSinceLastState, channel);
}

void CApiTestRunner::OnScreenShareSubscribeStateChanged(DingRtcEventHandler *handler, const char* uid, DingRtcSubscribeState oldState, DingRtcSubscribeState newState, int elapseSinceLastState, const char* channel) {
    TLOGI("OnScreenShareSubscribeStateChanged, uid: %s, oldstate: %d, newstate: %d, elapseSinceLastState: %d, channel: %s", uid, oldState, newState, elapseSinceLastState, channel);
}

void CApiTestRunner::OnSubscribeStreamTypeChanged(DingRtcEventHandler *handler, const char* uid, DingRtcVideoStreamType oldStreamType, DingRtcVideoStreamType newStreamType, int elapseSinceLastState, const char* channel) {
    TLOGI("OnSubscribeStreamTypeChanged, uid: %s, oldStreamType: %d, newStreamType: %d, elapseSinceLastState: %d, channel: %s", uid, oldStreamType, newStreamType, elapseSinceLastState, channel);
}

void CApiTestRunner::OnRecordingDeviceAudioLevel(DingRtcEventHandler *handler, int level) {
    TLOGI("OnRecordingDeviceAudioLevel: level: %d", level);
}

void CApiTestRunner::OnPlayoutDeviceAudioLevel(DingRtcEventHandler *handler, int level) {
    TLOGI("OnPlayoutDeviceAudioLevel: level: %d", level);
}

void CApiTestRunner::OnAudioVolumeIndication(DingRtcEventHandler *handler, const DingRtcAudioVolumeInfo* speakers, unsigned int speakerNumber) {
    TLOGI("OnAudioVolumeIndication, speakerNumber: %d", speakerNumber);
}

void CApiTestRunner::OnAudioDeviceStateChanged(DingRtcEventHandler *handler, const char* deviceId, DingRtcDeviceType deviceType, DingRtcDeviceState deviceState) {
    TLOGI("OnAudioDeviceStateChanged: deviceId: %s, deviceType: %d, deviceState: %d", deviceId, deviceType, deviceState);
}

void CApiTestRunner::OnApiCalledExecuted(DingRtcEventHandler *handler, int error, const char* api, const char* result) {
    TLOGI("OnApiCalledExecuted, error: %d, api: %s, result: %s", error, api?api:"", result?result:"");
}

void CApiTestRunner::OnSnapshotComplete(DingRtcEventHandler *handler, const char* userId, DingRtcVideoTrack videoTrack, const char* filePath, int width, int height, bool success) {
    TLOGI("OnSnapshotComplete, ");
}

void CApiTestRunner::OnJoinChannelResult(int result, const char* channel, const char* userId, int elapsed) {
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

void CApiTestRunner::OnLeaveChannelResult(int result, const DingRtcStats* stats) {
    if (!thread_) {
        return;
    }
    thread_->PostDelayedTask([this, result]{
        TLOGI("join_leave_count_: %d", join_leave_count_);
        if (join_leave_count_ == -1 || join_leave_count_ > 0) {
            JoinChannel();
        }
    }, 1000);
}

void CApiTestRunner::OnRemoteUserOnLineNotify(const char* uid, int elapsed) {
    if (!thread_) {
        return;
    }
    std::string userId(uid);
    thread_->PostTask([this, userId, elapsed]{
        DingRtcUserInfo *userInfo = nullptr;
        if (DingRtc_GetUserInfo(rtc_handle_, userId.c_str(), &userInfo) == 0) {
            TLOGI("OnRemoteUserOnLineNotify GetUserInfo, userId: %s, displayName: %s, isOnline: %d, "
                "hasAudio: %d, hasCamera: %d, hasScreenSharing: %d, requestAudio: %d, requestCamera: %d, requestScreenSharing: %d, "
                "preferCameraType: %s, subScribedAudio: %d, subScribedCameraType: %s, subScribedScreenSharing: %d",
                userInfo->userId, userInfo->displayName, userInfo->isOnline,
                userInfo->hasAudio, userInfo->hasCamera, userInfo->hasScreenSharing,
                userInfo->requestAudio, userInfo->requestCamera, userInfo->requestScreenSharing,
                userInfo->preferCameraType, userInfo->subScribedAudio, userInfo->subScribedCameraType, userInfo->subScribedScreenSharing);
            DingRtc_UserInfo_Free(&userInfo, 1);
        }
    });
}

void CApiTestRunner::OnRemoteUserOffLineNotify(const char* uid, DingRtcUserOfflineReason reason) {

}

void CApiTestRunner::OnPlaybackAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame) {
    CApiTestRunner *p_this = (CApiTestRunner *)observer->userData;
    if (p_this) {
        p_this->OnPlaybackAudioFrame(frame);
    }
}

void CApiTestRunner::OnCapturedAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame) {
    CApiTestRunner *p_this = (CApiTestRunner *)observer->userData;
    if (p_this) {
        p_this->OnCapturedAudioFrame(frame);
    }
}

void CApiTestRunner::OnProcessCapturedAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame) {
    CApiTestRunner *p_this = (CApiTestRunner *)observer->userData;
    if (p_this) {
        p_this->OnProcessCapturedAudioFrame(frame);
    }
}

void CApiTestRunner::OnPublishAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame) {
    CApiTestRunner *p_this = (CApiTestRunner *)observer->userData;
    if (p_this) {
        p_this->OnPublishAudioFrame(frame);
    }
}

void CApiTestRunner::OnRemoteUserAudioFrame(DingRtcAudioFrameObserver* observer, const char *uid, DingRtcAudioFrame* frame) {
    CApiTestRunner *p_this = (CApiTestRunner *)observer->userData;
    if (p_this) {
        p_this->OnRemoteUserAudioFrame(uid, frame);
    }
}

void CApiTestRunner::OnPlaybackAudioFrame(DingRtcAudioFrame* frame) {
    if (!playback_audio_dump_file_) {
        char filename[1024] = {0};
        snprintf(filename, sizeof(filename), "%s/audio_playback_%d_%d_%s.pcm", audio_dump_dir_.c_str(),
            frame->samplesPerSec, frame->channels, tlog::DateTimeFormat().c_str());
        playback_audio_dump_file_ = fopen(filename, "wb+");
        TLOGI("playback audio dump file: %s", filename);
    }

    if (playback_audio_dump_file_) {
        int bytesToWrite = frame->samples * frame->channels * frame->bytesPerSample;
        if (bytesToWrite > 0) {
            fwrite(frame->buffer, 1, bytesToWrite, playback_audio_dump_file_);
        }
    }
}

void CApiTestRunner::OnCapturedAudioFrame(DingRtcAudioFrame* frame) {

}

void CApiTestRunner::OnProcessCapturedAudioFrame(DingRtcAudioFrame* frame) {

}

void CApiTestRunner::OnPublishAudioFrame(DingRtcAudioFrame* frame) {

}

void CApiTestRunner::OnRemoteUserAudioFrame(const char *uid, DingRtcAudioFrame* frame) {
    auto found = remote_user_audio_files_.find(uid);
    if (found == remote_user_audio_files_.end()) {
        char filename[1024] = {0};
        snprintf(filename, sizeof(filename), "%s/remote_user_audio_%s_%d_%d_%s.pcm", audio_dump_dir_.c_str(),
            uid, frame->samplesPerSec, frame->channels, tlog::DateTimeFormat().c_str());
        auto f = fopen(filename, "wb+");
        remote_user_audio_files_.emplace(std::make_pair(uid, f));
        TLOGI("remote user audio dump file: %s", filename);
    } else {
        if (found->second) {
            int bytesToWrite = frame->samples * frame->channels * frame->bytesPerSample;
            if (bytesToWrite > 0) {
                fwrite(frame->buffer, 1, bytesToWrite, found->second);
            }
        }
    }
}
