#include "multiengine_test_runner.h"
#include <string>
#include <utility>
#include "tlog/tlog.h"
#include "tlog/utils.h"
#include "engine_interface.h"
#include "rtcengine_helper.h"

using namespace ding::rtc;

class EngineCallback : public ding::rtc::RtcEngineEventListener
                     , public ding::rtc::RtcEngineAudioFrameObserver
                     , public ding::rtc::RtcEngineVideoFrameObserver {
public:
    EngineCallback(EngineContext *context) : context_(context) {}
    ~EngineCallback() {}

    // --- implements RtcEngineEventListener ---
    void OnJoinChannelResult(int result, const char *channel, const char *userId, int elapsed) override {
        TLOGI("engine[%p], OnJoinChannelResult, result: %d, channel: %s, userId: %s, elapsed: %d", context_->rtc_engine, result, channel, userId, elapsed);
        context_->runner->OnJoinChannelResult(context_, result, channel, userId, elapsed);
    }
    void OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats) override {
        TLOGI("engine[%p], OnLeaveChannelResult, result: %d", context_->rtc_engine, result);
    }
    void OnRemoteUserOnLineNotify(const char *uid, int elapsed) override {
        TLOGI("engine[%p], OnRemoteUserOnLineNotify, uid: %s, elapsed: %d", context_->rtc_engine, uid, elapsed);
    }
    void OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason) override {
        TLOGI("engine[%p], OnRemoteUserOffLineNotify, uid: %s, reason: %d", context_->rtc_engine, uid, reason);
    }
    void OnBye(ding::rtc::RtcEngineOnByeType code) override {
        TLOGI("engine[%p], OnBye, code: %d", context_->rtc_engine, code);
    }
    void OnConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status, ding::rtc::RtcEngineConnectionStatusChangeReason reason) override {
        TLOGI("engine[%p], OnConnectionStatusChange, status: %d, reason: %d", context_->rtc_engine, status, reason);
    }
    void OnNetworkQualityChanged(const char *uid, ding::rtc::RtcEngineNetworkQuality upQuality, ding::rtc::RtcEngineNetworkQuality downQuality) override {
        TLOGI("engine[%p], OnNetworkQualityChanged, uid: %s, up: %d, down: %d", context_->rtc_engine, uid, upQuality, downQuality);
    }
    void OnStatisticsLog(ding::rtc::String const &log) override {
        TLOGI("engine[%p], OnStatisticsLog, log: %s", context_->rtc_engine, log.c_str());
    }
    void OnOccurWarning(int warn, const char *msg) override {
        TLOGI("engine[%p], OnOccurWarning, warn: %d, msg: %s", context_->rtc_engine, warn, msg);
    }
    void OnOccurError(int error, const char *msg) override {
        TLOGI("engine[%p], OnOccurError, error: %d, msg: %s", context_->rtc_engine, error, msg);
    }
    void OnStatisticsLevelLog(int level, const ding::rtc::String &log) override {
        TLOGI("engine[%p], OnStatisticsLevelLog, level: %d, log: %s", context_->rtc_engine, level, log.c_str());
    }
    void OnQualityReport(const ding::rtc::QualityReport &quality) override {
        TLOGI("engine[%p], OnQualityReport: quality: %d", context_->rtc_engine, quality);
    }
    void OnLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats) override {
        TLOGI("engine[%p], OnLocalAudioStats, stats: %s", context_->rtc_engine, LocalAudioStatsToString(localAudioStats).c_str());
    }
    void OnLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats) override {
        TLOGI("engine[%p], OnLocalVideoStats, stats: %s", context_->rtc_engine, LocalVideoStatsToString(localVideoStats).c_str());
    }
    void OnRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats) override {
        TLOGI("engine[%p], OnRemoteAudioStats, stats: %s", context_->rtc_engine, RemoteAudioStatsToString(remoteAudioStats).c_str());
    }
    void OnRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats) override {
        TLOGI("engine[%p], OnRemoteVideoStats, stats: %s", context_->rtc_engine, RemoteVideoStatsToString(remoteVideoStats).c_str());
    }
    void OnStats(const ding::rtc::RtcEngineStats &stats) override {
        TLOGI("engine[%p], OnStats, stats: %s", context_->rtc_engine, RtcEngineStatsToString(stats).c_str());
    }
    void OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel) override {
        TLOGI("engine[%p], OnAudioPublishStateChanged, oldstate: %d, newstate: %d, channel: %s", context_->rtc_engine, oldState, newState, channel);
    }
    void OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel) override {
        TLOGI("engine[%p], OnVideoPublishStateChanged, oldstate: %d, newstate: %d, elapsed: %d, channel: %s", context_->rtc_engine, oldState, newState, elapseSinceLastState, channel);
    }
    void OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel) override {
        TLOGI("engine[%p], OnScreenSharePublishStateChanged, oldstate: %d, newstate: %d, elapsed: %d, channel: %s", context_->rtc_engine, oldState, newState, elapseSinceLastState, channel);
    }
    void OnRemoteTrackAvailableNotify(const char *uid, ding::rtc::RtcEngineAudioTrack audioTrack, ding::rtc::RtcEngineVideoTrack videoTrack) override {
        TLOGI("engine[%p], OnRemoteTrackAvailableNotify, uid: %s, audioTrack: %d, videoTrack: %d", context_->rtc_engine, uid, audioTrack, videoTrack);
    }
    void OnUserAudioMuted(const char *uid, bool isMute) override {
        TLOGI("engine[%p], OnUserAudioMuted, uid: %s, isMute: %d", context_->rtc_engine, uid, isMute);
    }
    void OnUserVideoMuted(const char *uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack) override {
        TLOGI("engine[%p], OnUserVideoMuted, uid: %s, isMute: %d, videoTrack: %d", context_->rtc_engine, uid, isMute, videoTrack);
    }
    void OnFirstRemoteVideoFrameDrawn(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int width, int height, int elapsed) override {
        TLOGI("engine[%p], OnFirstRemoteVideoFrameDrawn, uid: %s, videoTrack: %d, width: %d, height: %d, elapsed: %d", context_->rtc_engine, uid, videoTrack, width, height, elapsed);
    }
    void OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed) override {
        TLOGI("engine[%p], OnFirstLocalVideoFrameDrawn, width: %d, height: %d, elapsed: %d", context_->rtc_engine, width, height, elapsed);
    }
    void OnFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) override {
        TLOGI("engine[%p], OnFirstVideoPacketSend, videoTrack: %d, timeCost: %d", context_->rtc_engine, videoTrack, timeCost);
    }
    void OnFirstVideoPacketReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) override {
        TLOGI("engine[%p], OnFirstVideoPacketReceived, uid: %s, videoTrack: %d, timeCost: %d", context_->rtc_engine, uid, videoTrack, timeCost);
    }
    void OnFirstVideoFrameReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) override {
        TLOGI("engine[%p], OnFirstVideoFrameReceived, uid: %s, videoTrack: %d, timeCost: %d", context_->rtc_engine, uid, videoTrack, timeCost);
    }
    void OnFirstAudioPacketSend(int timeCost) override {
        TLOGI("engine[%p], OnFirstAudioPacketSend, timeCost: %d", context_->rtc_engine, timeCost);
    }
    void OnFirstAudioPacketReceived(const char *uid, int timeCost) override {
        TLOGI("engine[%p], OnFirstAudioPacketReceived, uid: %s, timeCost: %d", context_->rtc_engine, uid, timeCost);
    }
    void OnAudioSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel) override {
        TLOGI("engine[%p], OnAudioSubscribeStateChanged, uid: %s, oldstate: %d, newstate: %d, elapsed: %d, channel: %s", context_->rtc_engine, uid, oldState, newState, elapseSinceLastState, channel);
    }
    void OnVideoSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel) override {
        TLOGI("engine[%p], OnVideoSubscribeStateChanged, uid: %s, oldstate: %d, newstate: %d, elapsed: %d, channel: %s", context_->rtc_engine, uid, oldState, newState, elapseSinceLastState, channel);
    }
    void OnScreenShareSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel) override {
        TLOGI("engine[%p], OnScreenShareSubscribeStateChanged, uid: %s, oldstate: %d, newstate: %d, elapsed: %d, channel: %s", context_->rtc_engine, uid, oldState, newState, elapseSinceLastState, channel);
    }
    void OnSubscribeStreamTypeChanged(const char *uid, ding::rtc::RtcEngineVideoStreamType oldStreamType, ding::rtc::RtcEngineVideoStreamType newStreamType, int elapseSinceLastState, const char *channel) override {
        TLOGI("engine[%p], OnSubscribeStreamTypeChanged, uid: %s, old: %d, new: %d, elapsed: %d, channel: %s", context_->rtc_engine, uid, oldStreamType, newStreamType, elapseSinceLastState, channel);
    }
    void OnRecordingDeviceAudioLevel(int level) override {
        TLOGI("engine[%p], OnRecordingDeviceAudioLevel, level: %d", context_->rtc_engine, level);
    }
    void OnPlayoutDeviceAudioLevel(int level) override {
        TLOGI("engine[%p], OnPlayoutDeviceAudioLevel, level: %d", context_->rtc_engine, level);
    }
    void OnAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers, unsigned int speakerNumber) override {
        TLOGI("engine[%p], OnAudioVolumeIndication, speakerNumber: %d", context_->rtc_engine, speakerNumber);
    }
    void OnAudioDeviceStateChanged(const char *deviceId, ding::rtc::RtcEngineDeviceType deviceType, ding::rtc::RtcEngineDeviceState deviceState) override {
        TLOGI("engine[%p], OnAudioDeviceStateChanged: deviceId: %s, deviceType: %d, deviceState: %d", context_->rtc_engine, deviceId, deviceType, deviceState);
    }
    void OnApiCalledExecuted(int error, const char *api, const char *result) override {
        TLOGI("engine[%p], OnApiCalledExecuted, error: %d, api: %s, result: %s", context_->rtc_engine, error, api?api:"", result?result:"");
    }
    void OnSnapshotComplete(const char *userId, ding::rtc::RtcEngineVideoTrack videoTrack, const char *filePath, int width, int height, bool success) override {}

    // --- implements RtcEngineAudioFrameObserver ---
    void OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {
        if (!context_->playback_audio_dump_file) {
            std::string audio_dump_dir = tlog::CurrentWorkingDirectory() + "/audio_dump_" + std::to_string(context_->engine_id);
            TLOGI("engine[%p], audio_dump_dir: %s", context_->rtc_engine, audio_dump_dir.c_str());
            tlog::CreateDirectory(audio_dump_dir.c_str());

            char filename[1024] = {0};
            snprintf(filename, sizeof(filename), "%s/audio_playback_%d_%d_%s.pcm", audio_dump_dir.c_str(),
                frame.samplesPerSec, frame.channels, tlog::DateTimeFormat().c_str());
            context_->playback_audio_dump_file = fopen(filename, "wb+");
            TLOGI("engine[%p], playback audio dump file: %s", context_->rtc_engine, filename);
        }
        if (context_->playback_audio_dump_file) {
            int bytesToWrite = frame.samples * frame.channels * frame.bytesPerSample;
            if (bytesToWrite > 0) {
                fwrite(frame.buffer, 1, bytesToWrite, context_->playback_audio_dump_file);
            }
        }
    }
    void OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}
    void OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}
    void OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}

    // --- implements RtcEngineVideoFrameObserver ---
    ding::rtc::RtcEngineVideoPixelFormat GetVideoFormatPreference() override {
        return RtcEngineVideoI420;
    }
    bool OnCaptureVideoFrame(ding::rtc::RtcEngineVideoFrame &frame) override { return true; }
    bool OnRemoteVideoFrame(ding::rtc::String uid, ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override {
        if (!context_->remote_video_dump_file) {
            std::string video_dump_dir = tlog::CurrentWorkingDirectory() + "/video_dump_" + std::to_string(context_->engine_id);
            TLOGI("engine[%p], video_dump_dir: %s", context_->rtc_engine, video_dump_dir.c_str());
            tlog::CreateDirectory(video_dump_dir.c_str());

            char filename[1024] = {0};
            snprintf(filename, sizeof(filename), "%s/remote_video_%s_%d_%d_%s.yuv", video_dump_dir.c_str(),
                uid.c_str(), frame.width, frame.height, tlog::DateTimeFormat().c_str());
            context_->remote_video_dump_file = fopen(filename, "wb+");
            TLOGI("engine[%p], remote video dump file: %s", context_->rtc_engine, filename);
        }

        if (context_->remote_video_dump_file) {
            int bytesToWrite = frame.width * frame.height * 3 / 2;
            if (bytesToWrite > 0) {
                fwrite(frame.data, 1, bytesToWrite, context_->remote_video_dump_file);
            }
        }
        return true;
    }
    bool OnPreEncodeVideoFrame(ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override { return true; }

    void OnPreDecodeVideoFrame(
        ding::rtc::String uid,
	ding::rtc::RtcEngineVideoTrack track,
	ding::rtc::RtcEngineEncodedVideoFrame &frame) override
    {
	printf("got one video frame\n");
    }
private:
    EngineContext *context_;
};

int MultiEngineTestRunner::s_engine_id_ = 0;

MultiEngineTestRunner::MultiEngineTestRunner(AppOptions* options)
    : TestingRunner(options) {
    TLOGI("MultiEngineTestRunner ctor");
}

MultiEngineTestRunner::~MultiEngineTestRunner() {
    TLOGI("MultiEngineTestRunner dtor");
    Stop();
}

bool MultiEngineTestRunner::Start() {
    TLOGI("MultiEngineTestRunner::Start");
    ResetCompleted();
    thread_.reset(new tlog::Thread());
    thread_->Start();
    thread_->PostTask([this]{
        TestingEntry();
    });
    return true;
}

void MultiEngineTestRunner::Stop() {
    TLOGI("MultiEngineTestRunner::Stop");
    thread_.reset(nullptr);
    for (auto context: contexts_) {
        FreeEngineContext(context);
    }
    contexts_.clear();
}

void MultiEngineTestRunner::TestingEntry() {
    // parse app config file
    if (!ParseMultiAppConfig(app_options_->app_config_filepath, app_configs_)) {
        TLOGE("ParseMultiAppConfig failed");
        return;
    }
    
    // config logging
    std::string cwd = tlog::CurrentWorkingDirectory();
    std::string log_dir = GetLogDir();
    TLOGI("set rtcengine log_dir: %s", log_dir.c_str());
    RtcEngine::SetLogLevel(ding::rtc::RtcEngineLogLevelInfo);
    RtcEngine::SetLogDirPath(log_dir.c_str());

    for (int i = 0; i < app_configs_.size(); i++) {
        AppConfig &app_config = app_configs_[i];
        EngineContext *context = CreateEngineContext(&app_config);
        TLOGI("CreateEngineContext, engine[%d]: %p", i, context->rtc_engine);
        contexts_.push_back(context);

        // request auth info
        RtcEngineAuthInfo auth_info;
        bool auth_ok = RequestAuthInfo(app_config.appServer,
                                    app_config.appId,
                                    app_config.userId,
                                    app_config.userName,
                                    app_config.channelId,
                                    app_config.env,
                                    app_config.passwd,
                                    auth_info);
        // if auth failed, exit
        if (!auth_ok) {
            TLOGE("RequestAuthInfo failed");
            return;
        }
        TLOGI("RequestAuthInfo result: ");
        TLOGI("    appId: %s", auth_info.appId.c_str());
        TLOGI("    channelId: %s", auth_info.channelId.c_str());
        TLOGI("    userId: %s", auth_info.userId.c_str());
        TLOGI("    token: %s", auth_info.token.c_str());
        TLOGI("    gslbServer: %s", auth_info.gslbServer.c_str());
        context->auth_info = auth_info;
        
        // join channel
        JoinChannel(context);
    }
}


EngineContext* MultiEngineTestRunner::CreateEngineContext(AppConfig *app_config) {
    nlohmann::json j_extras;
    j_extras["disableLogToConsole"] = true; // disable sdk log to console
    std::string extras = j_extras.dump();

    RtcEngine *rtc_engine = RtcEngine::Create(extras.c_str());

    EngineContext *context = new EngineContext();
    context->runner = this;
    context->app_config = app_config;
    context->rtc_engine = rtc_engine;
    context->engine_id = ++s_engine_id_;
    context->callback = new EngineCallback(context);
    if (app_config->pushExternalAudio || app_config->pushExternalVideo) {
        context->external_source_pusher = new ExternalSourcePusher(rtc_engine);
    }
    return context;
}

void MultiEngineTestRunner::FreeEngineContext(EngineContext *context) {
    LeaveChannel(context);
    if (context->external_source_pusher) {
        delete context->external_source_pusher;
        context->external_source_pusher = nullptr;
    }
    if (context->rtc_engine) {
        ding::rtc::RtcEngine::Destroy(context->rtc_engine);
        context->rtc_engine = nullptr;
    }
    if (context->callback) {
        delete context->callback;
        context->callback = nullptr;
    }
    if (context->playback_audio_dump_file) {
        fclose(context->playback_audio_dump_file);
        context->playback_audio_dump_file = nullptr;
    }
    if (context->remote_video_dump_file) {
        fclose(context->remote_video_dump_file);
        context->remote_video_dump_file = nullptr;
    }
    delete context;
}

int MultiEngineTestRunner::JoinChannel(EngineContext *context) {
    RtcEngine *rtc_engine = context->rtc_engine;
    AppConfig *app_config = context->app_config;

    rtc_engine->SetEngineEventListener(context->callback);

    rtc_engine->PublishLocalAudioStream(app_config->publishAudio);
    rtc_engine->PublishLocalVideoStream(app_config->publishVideo);

    rtc_engine->SubscribeAllRemoteAudioStreams(app_config->subscribeAllAudio);
    //rtc_engine->SubscribeAllRemoteVideoStreams(app_config->subscribeAllVideo);
    rtc_engine->SubscribeAllRemoteVideoStreams(app_config->subscribeAllCameraTracks, app_config->subscribeAllScreenTracks);
    ding::rtc::RtcEngineVideoStreamType restype;
    if (app_config->subscribeCameraTrackResolution == "LD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeLD;
    }
    else if (app_config->subscribeCameraTrackResolution == "SD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeSD;
    }
    else if (app_config->subscribeCameraTrackResolution == "HD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeHD;
    }
    else if (app_config->subscribeCameraTrackResolution == "FHD") {
        restype = ding::rtc::RtcEngineVideoStreamType::RtcEngineVideoStreamTypeFHD;
    }
    rtc_engine->SetRemoteDefaultVideoStreamType(restype);

    RtcEngineVideoEncoderConfiguration video_encoder_config;
    video_encoder_config.dimensions = RtcEngineVideoDimensions(app_config->videoWidth, app_config->videoHeight);
    video_encoder_config.frameRate = app_config->videoFps;
    rtc_engine->SetVideoEncoderConfiguration(video_encoder_config);

    if (app_config->dumpPlaybackAudio) {
        rtc_engine->RegisterAudioFrameObserver(context->callback);
        rtc_engine->EnableAudioFrameObserver(true, RtcEngineAudioPositionPlayback);
    }
    if (app_config->dumpRemoteVideo) {
        rtc_engine->RegisterVideoFrameObserver(context->callback);
        //rtc_engine->EnableVideoFrameObserver(true, RtcEnginePositionPreRender);
        rtc_engine->EnableVideoFrameObserver(true, RtcEnginePositionPreAndSkipDecoder);
    }

    return rtc_engine->JoinChannel(context->auth_info, app_config->userName.c_str());
}

void MultiEngineTestRunner::LeaveChannel(EngineContext *context) {
    if (context->external_source_pusher) {
        context->external_source_pusher->StopAll();
    }
    if (context->rtc_engine) {
        context->rtc_engine->LeaveChannel();
    }
}

void MultiEngineTestRunner::OnJoinChannelResult(EngineContext *context, int result, const char *channel, const char *userId, int elapsed) {
    thread_->PostTask([this, context, result]{
        if (result != 0) {
            // SetCompleted();
            return;
        }
        if (!context->app_config->pushExternalAudio && !context->app_config->pushExternalVideo) {
            //LeaveChannel(context);
            return;
        }
        if (context->app_config->pushExternalAudio) {
            RawAudioFileConfig audio_config;
            audio_config.file_path = context->app_config->pcmFilePath;
            audio_config.sample_rate = context->app_config->pcmSampleRate;
            audio_config.channels = context->app_config->pcmChannels;
            audio_config.freq = context->app_config->pcmReadFreq;
            if (context->external_source_pusher) {
                context->external_source_pusher->StartPushRawAuidoFile(audio_config, context->app_config->loopPlayCount);
            }
        }
        if (context->app_config->pushExternalVideo) {
            RawVideoFileConfig video_config;
            video_config.file_path = context->app_config->videoFilePath;
            video_config.pixel_format = context->app_config->videoPixelFormat;
            video_config.width = context->app_config->videoWidth;
            video_config.height = context->app_config->videoHeight;
            video_config.fps = context->app_config->videoFps;
            video_config.rotation = context->app_config->videoRotation;
            if (context->external_source_pusher) {
                context->external_source_pusher->StartPushRawVideoFile(video_config, context->app_config->loopPlayCount);
            }
        }
        CheckPushExternalStatus(context);
    });
}

void MultiEngineTestRunner::CheckPushExternalStatus(EngineContext *context) {
    if (context->external_source_pusher &&
        (context->external_source_pusher->IsRawAudioPushing() || context->external_source_pusher->IsRawVideoPushing())) {
        if (thread_) {
            thread_->PostDelayedTask([this, context] { CheckPushExternalStatus(context); }, 1000);
        }
    } else {
        //TLOGI("external source push finished, then leave channel");
        // finally leave channel
        //LeaveChannel(context);
    }
}
