#ifndef __HTTP_SERVER_TEST_RUNNER_H__
#define __HTTP_SERVER_TEST_RUNNER_H__

#include <memory>
#include "app_config.h"
#include "engine_interface.h"
#include "testing_runner.h"
#include "http_server.h"

class ExternalSourcePusher;

class HttpServerTestRunner : public TestingRunner
                           , public HttpServerCallback
                           , public ding::rtc::RtcEngineEventListener
                           , public ding::rtc::RtcEngineAudioFrameObserver {
public:
    HttpServerTestRunner(AppOptions *options);
    virtual ~HttpServerTestRunner();

    // --- implements TestingRunner ---
    bool Start() override;
    void Stop() override;

    // --- implements HttpServerCallback ---
    void OnHttpRequest(const std::string &method,
                       const std::string &uri,
                       const std::unordered_map<std::string, std::string> &headers,
                       const std::string &body) override;
    void OnWsConnected() override;
    void OnWsMessage(const std::string &msg) override;
    void OnConnectionClose() override;

    // --- implements RtcEngineEventListener ---
    virtual void OnJoinChannelResult(int result, const char *channel, const char *userId, int elapsed);
    virtual void OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats);
    virtual void OnRemoteUserOnLineNotify(const char *uid, int elapsed);
    virtual void OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason);
    virtual void OnBye(ding::rtc::RtcEngineOnByeType code);
    virtual void OnConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status,
                                           ding::rtc::RtcEngineConnectionStatusChangeReason reason);
    virtual void OnNetworkQualityChanged(const char *uid,
                                         ding::rtc::RtcEngineNetworkQuality upQuality,
                                         ding::rtc::RtcEngineNetworkQuality downQuality);
    virtual void OnStatisticsLog(ding::rtc::String const &log);
    virtual void OnOccurWarning(int warn, const char *msg);
    virtual void OnOccurError(int error, const char *msg);
    virtual void OnStatisticsLevelLog(int level, const ding::rtc::String &log);
    virtual void OnQualityReport(const ding::rtc::QualityReport &quality);
    virtual void OnLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats);
    virtual void OnLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats);
    virtual void OnRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats);
    virtual void OnRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats);
    virtual void OnStats(const ding::rtc::RtcEngineStats &stats);
    virtual void OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                            ding::rtc::RtcEnginePublishState newState,
                                            int elapseSinceLastState,
                                            const char *channel);
    virtual void OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                            ding::rtc::RtcEnginePublishState newState,
                                            int elapseSinceLastState,
                                            const char *channel);
    virtual void OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState,
                                                  ding::rtc::RtcEnginePublishState newState,
                                                  int elapseSinceLastState,
                                                  const char *channel);
    virtual void OnRemoteTrackAvailableNotify(const char *uid,
                                              ding::rtc::RtcEngineAudioTrack audioTrack,
                                              ding::rtc::RtcEngineVideoTrack videoTrack);
    virtual void OnUserAudioMuted(const char *uid, bool isMute);
    virtual void OnUserVideoMuted(const char *uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack);
    virtual void OnFirstRemoteVideoFrameDrawn(const char *uid,
                                              ding::rtc::RtcEngineVideoTrack videoTrack,
                                              int width,
                                              int height,
                                              int elapsed);
    virtual void OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed);
    virtual void OnFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost);
    virtual void OnFirstVideoPacketReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost);
    virtual void OnFirstVideoFrameReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost);
    virtual void OnFirstAudioPacketSend(int timeCost);
    virtual void OnFirstAudioPacketReceived(const char *uid, int timeCost);
    virtual void OnAudioSubscribeStateChanged(const char *uid,
                                              ding::rtc::RtcEngineSubscribeState oldState,
                                              ding::rtc::RtcEngineSubscribeState newState,
                                              int elapseSinceLastState,
                                              const char *channel);
    virtual void OnVideoSubscribeStateChanged(const char *uid,
                                              ding::rtc::RtcEngineSubscribeState oldState,
                                              ding::rtc::RtcEngineSubscribeState newState,
                                              int elapseSinceLastState,
                                              const char *channel);
    virtual void OnScreenShareSubscribeStateChanged(const char *uid,
                                                    ding::rtc::RtcEngineSubscribeState oldState,
                                                    ding::rtc::RtcEngineSubscribeState newState,
                                                    int elapseSinceLastState,
                                                    const char *channel);
    virtual void OnSubscribeStreamTypeChanged(const char *uid,
                                              ding::rtc::RtcEngineVideoStreamType oldStreamType,
                                              ding::rtc::RtcEngineVideoStreamType newStreamType,
                                              int elapseSinceLastState,
                                              const char *channel);
    virtual void OnRecordingDeviceAudioLevel(int level);
    virtual void OnPlayoutDeviceAudioLevel(int level);
    virtual void OnAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers, unsigned int speakerNumber);
    virtual void OnAudioDeviceStateChanged(const char *deviceId,
                                           ding::rtc::RtcEngineDeviceType deviceType,
                                           ding::rtc::RtcEngineDeviceState deviceState);
#if defined(ALI_OS_ANDROID) || defined(ALI_OS_IOS)
    virtual void OnAudioRouteChanged(ding::rtc::RtcEngineAudioRouteType audioRouteType);
#endif
    virtual void OnApiCalledExecuted(int error, const char *api, const char *result);
    virtual void OnSnapshotComplete(const char *userId,
                                    ding::rtc::RtcEngineVideoTrack videoTrack,
                                    const char *filePath,
                                    int width,
                                    int height,
                                    bool success);

    // --- implements RtcEngineAudioFrameObserver ---
    void OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;
    void OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}
    void OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}
    void OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override {}
    
private:
    bool InitEngine();
    void DestroyEngine();
    int JoinChannel();
    void LeaveChannel();
    bool StartPushExternalAudio();
    bool StartPushExternalVideo();
    void StopPushExternalAudio();
    void StopPushExternalVideo();

    void SendMessageToClient(nlohmann::json& j);

    std::unique_ptr<HttpServer> http_server_;
    AppConfig app_config_;
    ding::rtc::RtcEngine *rtc_engine_{nullptr};
    ding::rtc::RtcEngineAuthInfo auth_info_;
    std::unique_ptr<ExternalSourcePusher> external_source_pusher_;
    bool is_websocket_{false};

    std::string audio_dump_dir_;
    std::string video_dump_dir_;
    FILE *playback_audio_dump_file_{nullptr};
    FILE *remote_video_dump_file_{nullptr};
};

#endif // __HTTP_SERVER_TEST_RUNNER_H__
