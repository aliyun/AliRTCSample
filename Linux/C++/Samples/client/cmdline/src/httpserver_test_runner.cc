#include "httpserver_test_runner.h"
#include <sstream>
#include "cmdline/src/app_config.h"
#include "tlog/tlog.h"
#include "tlog/utils.h"
#include "nlohmann/json.hpp"
#include "engine_interface.h"
#include "rtcengine_helper.h"
#include "external_source_pusher.h"

using json = nlohmann::json;

static const char *kDefaultHttpServerUrl = "http://0.0.0.0:8000";

HttpServerTestRunner::HttpServerTestRunner(AppOptions *options)
    : TestingRunner(options) {
}
HttpServerTestRunner::~HttpServerTestRunner() {}

// --- implements TestingRunner ---
bool HttpServerTestRunner::Start() {
    http_server_.reset(new HttpServer());
    http_server_->SetCallback(this);
    http_server_->SetUrl(kDefaultHttpServerUrl);
    return http_server_->Start();
}

void HttpServerTestRunner::Stop() {
    LeaveChannel();
    DestroyEngine();
    http_server_->Stop();
    http_server_.reset(nullptr);
}

static inline std::string to_string(const std::unordered_map<std::string, std::string> &headers) {
    std::ostringstream oss;
    auto it = headers.begin();
    while (it != headers.end()) {
        oss << it->first << ": " << it->second;
        if (++it != headers.end()) {
            oss << ",";
        }
    }
    return oss.str();
}

// --- implements HttpServerCallback ---
void HttpServerTestRunner::OnHttpRequest(const std::string &method,
                                         const std::string &uri,
                                         const std::unordered_map<std::string, std::string> &headers,
                                         const std::string &body) {
    TLOGI("HttpServerTestRunner::OnHttpRequest, method: %s, uri: %s, body: %s, headers: %s",
          method.c_str(), uri.c_str(), body.c_str(), to_string(headers).c_str());
    if (uri == "/api") {
        json j_msg = json::parse(body, nullptr, false);
        if (!j_msg.is_discarded() && j_msg.contains("type")) {
            std::string msg_type = j_msg.value("type", "");
            if (msg_type == "joinChannel") {
                json j_app_config = j_msg["appConfig"];
                app_config_ = DefaultAppConfigForTest();
                from_json(j_app_config, app_config_);
                
                RtcEngineAuthInfo authInfo;
                bool authOK = RequestAuthInfo(app_config_.appServer,
                                            app_config_.appId,
                                            app_config_.userId,
                                            app_config_.userName,
                                            app_config_.channelId,
                                            app_config_.env,
                                            app_config_.passwd,
                                            authInfo);
                if (!authOK) {
                    TLOGE("request auth info failed");
                    return;
                }
                auth_info_ = authInfo;

                InitEngine();
                JoinChannel();
            } else if (msg_type == "leaveChannel") {
                LeaveChannel();
            }
        }
    }
}
void HttpServerTestRunner::OnWsConnected() {
    TLOGI("HttpServerTestRunner::OnWsConnected");
    is_websocket_ = true;
}

void HttpServerTestRunner::OnConnectionClose() {
    TLOGI("HttpServerTestRunner::OnConnectionClose");
    is_websocket_ = false;
}

void HttpServerTestRunner::OnWsMessage(const std::string &msg) {
    TLOGI("HttpServerTestRunner::OnWsMessage, msg: %s", msg.c_str());
    json j_msg = json::parse(msg, nullptr, false);
    if (j_msg.is_discarded()) {
        TLOGE("parse json failed");
        return;
    }
    if (!j_msg.contains("type")) {
        TLOGE("type field not found");
        return;
    }

    std::string msg_type = j_msg.value("type", "");
    if (msg_type == "joinChannel") {
        json j_app_config = j_msg["appConfig"];
        app_config_ = DefaultAppConfigForTest();
        from_json(j_app_config, app_config_);
        
        RtcEngineAuthInfo authInfo;
        bool authOK = RequestAuthInfo(app_config_.appServer,
                                    app_config_.appId,
                                    app_config_.userId,
                                    app_config_.userName,
                                    app_config_.channelId,
                                    app_config_.env,
                                    app_config_.passwd,
                                    authInfo);
        if (!authOK) {
            TLOGE("request auth info failed");
            return;
        }
        auth_info_ = authInfo;

        InitEngine();
        JoinChannel();
    } else if (msg_type == "leaveChannel") {
        LeaveChannel();
    }
}

bool HttpServerTestRunner::InitEngine() {
    if (rtc_engine_) {
        return true;
    }
    std::string cwd = tlog::CurrentWorkingDirectory();
    std::string log_dir = GetLogDir();
    TLOGI("set rtcengine log_dir: %s", log_dir.c_str());
    RtcEngine::SetLogDirPath(log_dir.c_str());

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
    rtc_engine_ = RtcEngine::Create(extras.c_str());
    rtc_engine_->SetEngineEventListener(this);

    // if (app_config_.dumpRemoteVideo) {
    //     videoFrameObserver = new RtcVideoFrameObserver();
    //     rtc_engine_->RegisterVideoFrameObserver(ctx->videoFrameObserver);
    // }
    if (app_config_.pushExternalAudio || app_config_.pushExternalVideo) {
        external_source_pusher_.reset(new ExternalSourcePusher(rtc_engine_));
    }

    return true;
}

void HttpServerTestRunner::DestroyEngine() {
    if (rtc_engine_) {
        rtc_engine_->SetEngineEventListener(nullptr);
        RtcEngine::Destroy(rtc_engine_);
        rtc_engine_ = nullptr;
    }
}

int HttpServerTestRunner::JoinChannel() {
    // NOTE: need reset pub/sub before join channel?
    rtc_engine_->PublishLocalAudioStream(app_config_.publishAudio);
    rtc_engine_->PublishLocalVideoStream(app_config_.publishVideo);

    rtc_engine_->SubscribeAllRemoteAudioStreams(app_config_.subscribeAllAudio);
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

    RtcEngineVideoEncoderConfiguration video_encoder_config;
    video_encoder_config.dimensions = RtcEngineVideoDimensions(app_config_.videoWidth, app_config_.videoHeight);
    video_encoder_config.frameRate = app_config_.videoFps;
    rtc_engine_->SetVideoEncoderConfiguration(video_encoder_config);

    if (app_config_.dumpPlaybackAudio) {
        audio_dump_dir_ = tlog::CurrentWorkingDirectory() + "/audio_dump";
        TLOGI("audio_dump_dir_: %s", audio_dump_dir_.c_str());
        tlog::CreateDirectory(audio_dump_dir_.c_str());
        rtc_engine_->RegisterAudioFrameObserver(this);
        rtc_engine_->EnableAudioFrameObserver(true, RtcEngineAudioPositionPlayback);
    }

    return rtc_engine_->JoinChannel(auth_info_, app_config_.userName.c_str());
}

void HttpServerTestRunner::LeaveChannel() {
    if (app_config_.pushExternalAudio) {
        StopPushExternalAudio();
    }
    if (app_config_.pushExternalVideo) {
        StopPushExternalVideo();
    }
    if (rtc_engine_) {
        rtc_engine_->LeaveChannel();
    }
}

bool HttpServerTestRunner::StartPushExternalAudio() {
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

bool HttpServerTestRunner::StartPushExternalVideo() {
    RawVideoFileConfig video_config;
    video_config.file_path = app_config_.videoFilePath;
    video_config.pixel_format = app_config_.videoPixelFormat;
    video_config.width = app_config_.videoWidth;
    video_config.height = app_config_.videoHeight;
    video_config.fps = app_config_.videoFps;
    if (external_source_pusher_) {
        return external_source_pusher_->StartPushRawVideoFile(video_config, app_config_.loopPlayCount);
    }
    return false;
}

void HttpServerTestRunner::StopPushExternalAudio() {
    if (external_source_pusher_) {
        external_source_pusher_->StopPushRawAuidoFile();
    }
}

void HttpServerTestRunner::StopPushExternalVideo() {
    if (external_source_pusher_) {
        external_source_pusher_->StopPushRawVideoFile();
    }
}

// --- implements RtcEngineEventListener ---
void HttpServerTestRunner::OnJoinChannelResult(int result, const char *channel, const char *userId, int elapsed) {
    TLOGI("OnJoinChannelResult: result: %d, channel: %s, userId: %s, elapsed: %d", result, channel, userId, elapsed);
    if (result == 0) {
        if (app_config_.pushExternalAudio) {
            StartPushExternalAudio();
        }
        if (app_config_.pushExternalVideo) {
            StartPushExternalVideo();
        }
    }

    json j_msg;
    j_msg["type"] = "OnJoinChannelResult";
    j_msg["result"] = result;
    j_msg["elapsed"] = elapsed;
    SendMessageToClient(j_msg);
}

void HttpServerTestRunner::OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats) {
    TLOGI("OnLeaveChannelResult: result: %d", result);
    json j_msg;
    j_msg["type"] = "OnLeaveChannelResult";
    j_msg["result"] = result;
    SendMessageToClient(j_msg);
}

void HttpServerTestRunner::OnRemoteUserOnLineNotify(const char *uid, int elapsed) {
    TLOGI("OnRemoteUserOnLineNotify, uid: %s, elapsed: %d", uid, elapsed);
    json j_msg;
    j_msg["type"] = "OnRemoteUserOnLineNotify";
    j_msg["uid"] = uid;
    j_msg["elapsed"] = elapsed;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason) {
    TLOGI("OnRemoteUserOffLineNotify, uid: %d, reason: %d", uid, reason);
    json j_msg;
    j_msg["type"] = "OnRemoteUserOffLineNotify";
    j_msg["uid"] = uid;
    j_msg["reason"] = to_string(reason);
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnBye(ding::rtc::RtcEngineOnByeType code) {
    TLOGI("OnBye: code: %d", code);
}
void HttpServerTestRunner::OnConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status,
                                                  ding::rtc::RtcEngineConnectionStatusChangeReason reason) {
    TLOGI("OnConnectionStatusChange: status: %d, reason: %d", status, reason);
}
void HttpServerTestRunner::OnNetworkQualityChanged(const char *uid,
                                                ding::rtc::RtcEngineNetworkQuality upQuality,
                                                ding::rtc::RtcEngineNetworkQuality downQuality) {
    TLOGI("OnNetworkQualityChanged: uid: %s, up: %d, down: %d", uid, upQuality, downQuality);
}
void HttpServerTestRunner::OnStatisticsLog(ding::rtc::String const &log) {
    TLOGI("OnStatisticsLog: log: %s", log.c_str());
}
void HttpServerTestRunner::OnOccurWarning(int warn, const char *msg) {
    TLOGI("OnOccurWarning: warn: %d, msg: %s", warn, msg);
}
void HttpServerTestRunner::OnOccurError(int error, const char *msg) {
    TLOGI("OnOccurError: error: %d, msg: %s", error, msg);
}
void HttpServerTestRunner::OnStatisticsLevelLog(int level, const ding::rtc::String &log) {
    TLOGI("OnStatisticsLevelLog: level: %d, log: %s", level, log.c_str());
}
void HttpServerTestRunner::OnQualityReport(const ding::rtc::QualityReport &quality) {
    TLOGI("OnQualityReport: quality: %d", quality);
}
void HttpServerTestRunner::OnLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats) {
    TLOGI("OnLocalAudioStats, stats: %s", LocalAudioStatsToString(localAudioStats).c_str());
}
void HttpServerTestRunner::OnLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats) {
    TLOGI("OnLocalVideoStats, stats: %s", LocalVideoStatsToString(localVideoStats).c_str());
}
void HttpServerTestRunner::OnRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats) {
    TLOGI("OnRemoteAudioStats, stats: %s", RemoteAudioStatsToString(remoteAudioStats).c_str());
}
void HttpServerTestRunner::OnRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats) {
    TLOGI("OnRemoteVideoStats, stats: %s", RemoteVideoStatsToString(remoteVideoStats).c_str());
}
void HttpServerTestRunner::OnStats(const ding::rtc::RtcEngineStats &stats) {
    TLOGI("OnStats, stats: %s", RtcEngineStatsToString(stats).c_str());
}
void HttpServerTestRunner::OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                   ding::rtc::RtcEnginePublishState newState,
                                                   int elapseSinceLastState,
                                                   const char *channel) {
    TLOGI("OnAudioPublishStateChanged: oldstate: %d, newstate: %d, channel: %s", oldState, newState, channel);
    json j_msg;
    j_msg["type"] = "OnAudioPublishStateChanged";
    j_msg["oldState"] = to_string(oldState);
    j_msg["newState"] = to_string(newState);
    j_msg["elapsed"] = elapseSinceLastState;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                   ding::rtc::RtcEnginePublishState newState,
                                                   int elapseSinceLastState,
                                                   const char *channel) {
    TLOGI("OnVideoPublishStateChanged: oldstate: %d, newstate: %d, elapsed: %d, channel: %s",
          oldState,
          newState,
          elapseSinceLastState,
          channel);
    json j_msg;
    j_msg["type"] = "OnVideoPublishStateChanged";
    j_msg["oldState"] = to_string(oldState);
    j_msg["newState"] = to_string(newState);
    j_msg["elapsed"] = elapseSinceLastState;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                         ding::rtc::RtcEnginePublishState newState,
                                                         int elapseSinceLastState,
                                                         const char *channel) {
    TLOGI("OnScreenSharePublishStateChanged: oldstate: %d, newstate: %d, elapsed: %d, channel: %s",
          oldState,
          newState,
          elapseSinceLastState,
          channel);
    json j_msg;
    j_msg["type"] = "OnScreenSharePublishStateChanged";
    j_msg["oldState"] = to_string(oldState);
    j_msg["newState"] = to_string(newState);
    j_msg["elapsed"] = elapseSinceLastState;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnRemoteTrackAvailableNotify(const char *uid,
                                                     ding::rtc::RtcEngineAudioTrack audioTrack,
                                                     ding::rtc::RtcEngineVideoTrack videoTrack) {
    TLOGI("OnRemoteTrackAvailableNotify: uid: %s, audioTrack: %d, videoTrack: %d", uid, audioTrack, videoTrack);
}
void HttpServerTestRunner::OnUserAudioMuted(const char *uid, bool isMute) {
    TLOGI("OnUserAudioMuted: uid: %s, isMute: %d", uid, isMute);
    json j_msg;
    j_msg["type"] = "OnUserAudioMuted";
    j_msg["uid"] = uid;
    j_msg["isMute"] = isMute;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnUserVideoMuted(const char *uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack) {
    TLOGI("OnUserVideoMuted: uid: %s, isMute: %d, videoTrack: %d", uid, isMute, videoTrack);
    json j_msg;
    j_msg["type"] = "OnUserVideoMuted";
    j_msg["uid"] = uid;
    j_msg["isMute"] = isMute;
    j_msg["videoTrack"] = to_string(videoTrack);
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnFirstRemoteVideoFrameDrawn(const char *uid,
                                                     ding::rtc::RtcEngineVideoTrack videoTrack,
                                                     int width,
                                                     int height,
                                                     int elapsed) {
    TLOGI("OnFirstRemoteVideoFrameDrawn: uid: %s, videoTrack: %d, width: %d, height: %d, elapsed: %d",
          uid, videoTrack, width, height, elapsed);
    json j_msg;
    j_msg["type"] = "OnFirstRemoteVideoFrameDrawn";
    j_msg["uid"] = uid;
    j_msg["videoTrack"] = to_string(videoTrack);
    j_msg["width"] = width;
    j_msg["height"] = height;
    j_msg["elapsed"] = elapsed;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed) {
    TLOGI("OnFirstLocalVideoFrameDrawn:width: %d, height: %d, elapsed: %d", width, height, elapsed);
    json j_msg;
    j_msg["type"] = "OnFirstLocalVideoFrameDrawn";
    j_msg["width"] = width;
    j_msg["height"] = height;
    j_msg["elapsed"] = elapsed;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) {
    TLOGI("OnFirstVideoPacketSend, videoTrack: %d, timeCost: %d", videoTrack, timeCost);
    json j_msg;
    j_msg["type"] = "OnFirstVideoPacketSend";
    j_msg["videoTrack"] = to_string(videoTrack);
    j_msg["timeCost"] = timeCost;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnFirstVideoPacketReceived(const char *uid,
                                                   ding::rtc::RtcEngineVideoTrack videoTrack,
                                                   int timeCost) {
    TLOGI("OnFirstVideoPacketReceived, uid: %s, videoTrack: %d, timeCost: %d", uid, videoTrack, timeCost);
    json j_msg;
    j_msg["type"] = "OnFirstVideoPacketReceived";
    j_msg["uid"] = uid;
    j_msg["videoTrack"] = to_string(videoTrack);
    j_msg["timeCost"] = timeCost;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnFirstVideoFrameReceived(const char *uid,
                                                  ding::rtc::RtcEngineVideoTrack videoTrack,
                                                  int timeCost) {
    TLOGI("OnFirstVideoFrameReceived, uid: %s, videoTrack: %d, timeCost: %d", uid, videoTrack, timeCost);
    json j_msg;
    j_msg["type"] = "OnFirstVideoFrameReceived";
    j_msg["uid"] = uid;
    j_msg["videoTrack"] = to_string(videoTrack);
    j_msg["timeCost"] = timeCost;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnFirstAudioPacketSend(int timeCost) {
    TLOGI("OnFirstAudioPacketSend, timeCost: %d", timeCost);
    json j_msg;
    j_msg["type"] = "OnFirstAudioPacketSend";
    j_msg["timeCost"] = timeCost;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnFirstAudioPacketReceived(const char *uid, int timeCost) {
    TLOGI("OnFirstAudioPacketReceived, uid: %d, timeCost: %d", timeCost);
    json j_msg;
    j_msg["type"] = "OnFirstAudioPacketReceived";
    j_msg["uid"] = uid;
    j_msg["timeCost"] = timeCost;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnAudioSubscribeStateChanged(const char *uid,
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
    json j_msg;
    j_msg["type"] = "OnAudioSubscribeStateChanged";
    j_msg["uid"] = uid;
    j_msg["oldState"] = to_string(oldState);
    j_msg["newState"] = to_string(newState);
    j_msg["elapsed"] = elapseSinceLastState;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnVideoSubscribeStateChanged(const char *uid,
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
    json j_msg;
    j_msg["type"] = "OnVideoSubscribeStateChanged";
    j_msg["uid"] = uid;
    j_msg["oldState"] = to_string(oldState);
    j_msg["newState"] = to_string(newState);
    j_msg["elapsed"] = elapseSinceLastState;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnScreenShareSubscribeStateChanged(const char *uid,
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
    json j_msg;
    j_msg["type"] = "OnScreenShareSubscribeStateChanged";
    j_msg["uid"] = uid;
    j_msg["oldState"] = to_string(oldState);
    j_msg["newState"] = to_string(newState);
    j_msg["elapsed"] = elapseSinceLastState;
    SendMessageToClient(j_msg);
}
void HttpServerTestRunner::OnSubscribeStreamTypeChanged(const char *uid,
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
void HttpServerTestRunner::OnRecordingDeviceAudioLevel(int level) {
    TLOGI("OnRecordingDeviceAudioLevel: level: %d", level);
}
void HttpServerTestRunner::OnPlayoutDeviceAudioLevel(int level) {
    TLOGI("OnPlayoutDeviceAudioLevel: level: %d", level);
}
void HttpServerTestRunner::OnAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers,
                                                unsigned int speakerNumber) {
    TLOGI("OnAudioVolumeIndication, ");
}
void HttpServerTestRunner::OnAudioDeviceStateChanged(const char *deviceId,
                                                  ding::rtc::RtcEngineDeviceType deviceType,
                                                  ding::rtc::RtcEngineDeviceState deviceState) {
    TLOGI("OnAudioDeviceStateChanged: deviceId: %s, deviceType: %d, deviceState: %d", deviceId, deviceType, deviceState);
}
void HttpServerTestRunner::OnApiCalledExecuted(int error, const char *api, const char *result) {
    TLOGI("OnApiCalledExecuted, error: %d, api: %s, result: %s", error, api?api:"", result?result:"");
}
void HttpServerTestRunner::OnSnapshotComplete(const char *userId,
                                           ding::rtc::RtcEngineVideoTrack videoTrack,
                                           const char *filePath,
                                           int width,
                                           int height,
                                           bool success) {
    TLOGI("OnSnapshotComplete, ");
}

void HttpServerTestRunner::OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) {
    if (!playback_audio_dump_file_) {
        char filename[1024] = {0};
        snprintf(filename, sizeof(filename), "%s/audio_playback_%d_%d_%s.pcm", audio_dump_dir_.c_str(),
            frame.samplesPerSec, frame.channels, tlog::DateTimeFormat().c_str());
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

void HttpServerTestRunner::SendMessageToClient(nlohmann::json &j) {
    if (http_server_) {
        j["datetime"] = tlog::DateTimePretty();
        if (is_websocket_) {
            http_server_->SendWsMessage(j.dump());
        } else {
            http_server_->SendHttpResponse(200, j.dump());
        }
    }
}
