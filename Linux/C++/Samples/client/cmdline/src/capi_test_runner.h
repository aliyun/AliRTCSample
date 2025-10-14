#ifndef __CAPI_TEST_RUNNER_H__
#define __CAPI_TEST_RUNNER_H__

#include <memory>
#include <string>
#include "testing_runner.h"
#include "app_config.h"
#include "../../../src/c_api/dingrtc_api.h"
#include "tlog/thread.h"
#include "raw_file_config.h"

class ExternalSourcePusherC;

class CApiTestRunner : public TestingRunner {
public:
    CApiTestRunner(AppOptions* options);
    virtual ~CApiTestRunner();

    // --- implements TestingRunner ---
    bool Start() override;
    void Stop() override;

private:
    void TestingEntry();
    bool InitEngine();
    void DestroyEngine();
    int JoinChannel();
    void LeaveChannel();
    bool StartPushExternalAudio();
    bool StartPushExternalVideo();
    void StopPushExternalAudio();
    void StopPushExternalVideo();
    void CheckPushExternalStatus();

    void OnJoinChannelResult(int result, const char* channel, const char* userId, int elapsed);
    void OnLeaveChannelResult(int result, const DingRtcStats* stats);
    void OnRemoteUserOnLineNotify(const char* uid, int elapsed);
    void OnRemoteUserOffLineNotify(const char* uid, DingRtcUserOfflineReason reason);

    // --- DingRtcEventHandler
    static void OnJoinChannelResult(DingRtcEventHandler *handler, int result, const char* channel, const char* userId, int elapsed);
    static void OnLeaveChannelResult(DingRtcEventHandler *handler, int result, const DingRtcStats* stats);
    static void OnRemoteUserOnLineNotify(DingRtcEventHandler *handler, const char* uid, int elapsed);
    static void OnRemoteUserOffLineNotify(DingRtcEventHandler *handler, const char* uid, DingRtcUserOfflineReason reason);
    static void OnBye(DingRtcEventHandler *handler, DingRtcOnByeType code);
    static void OnConnectionStatusChanged(DingRtcEventHandler *handler, DingRtcConnectionStatus status, DingRtcConnectionStatusChangeReason reason);
    static void OnNetworkQualityChanged(DingRtcEventHandler *handler, const char* uid, DingRtcNetworkQuality upQuality,DingRtcNetworkQuality downQuality);
    static void OnStatisticsLog(DingRtcEventHandler *handler, const char* log);
    static void OnOccurWarning(DingRtcEventHandler *handler, int warn, const char* msg);
    static void OnOccurError(DingRtcEventHandler *handler, int error, const char* msg);
    static void OnStatisticsLevelLog(DingRtcEventHandler *handler, int level, const char* log);
    static void OnQualityReport(DingRtcEventHandler *handler, const DingRtcQualityReport* quality);
    static void OnLocalAudioStats(DingRtcEventHandler *handler, const DingRtcLocalAudioStats* localAudioStats);
    static void OnLocalVideoStats(DingRtcEventHandler *handler, const DingRtcLocalVideoStats* localVideoStats);
    static void OnRemoteAudioStats(DingRtcEventHandler *handler, const DingRtcRemoteAudioStats* remoteAudioStats);
    static void OnRemoteVideoStats(DingRtcEventHandler *handler, const DingRtcRemoteVideoStats* remoteVideoStats);
    static void OnStats(DingRtcEventHandler *handler, const DingRtcStats* stats);
    static void OnAudioPublishStateChanged(DingRtcEventHandler *handler, DingRtcPublishState oldState, DingRtcPublishState newState, int elapseSinceLastState, const char* channel);
    static void OnVideoPublishStateChanged(DingRtcEventHandler *handler, DingRtcPublishState oldState, DingRtcPublishState newState, int elapseSinceLastState, const char* channel);
    static void OnScreenSharePublishStateChanged(DingRtcEventHandler *handler, DingRtcPublishState oldState, DingRtcPublishState newState, int elapseSinceLastState, const char* channel);
    static void OnRemoteTrackAvailableNotify(DingRtcEventHandler *handler, const char* uid, DingRtcAudioTrack audioTrack, DingRtcVideoTrack videoTrack);
    static void OnUserAudioMuted(DingRtcEventHandler *handler, const char* uid, bool isMute);
    static void OnUserVideoMuted(DingRtcEventHandler *handler, const char* uid, bool isMute, DingRtcVideoTrack videoTrack);
    static void OnFirstRemoteVideoFrameDrawn(DingRtcEventHandler *handler, const char* uid, DingRtcVideoTrack videoTrack, int width, int height, int elapsed);
    static void OnFirstLocalVideoFrameDrawn(DingRtcEventHandler *handler, int width, int height, int elapsed);
    static void OnFirstVideoPacketSend(DingRtcEventHandler *handler, DingRtcVideoTrack videoTrack, int timeCost);
    static void OnFirstVideoPacketReceived(DingRtcEventHandler *handler, const char* uid, DingRtcVideoTrack videoTrack, int timeCost);
    static void OnFirstVideoFrameReceived(DingRtcEventHandler *handler, const char* uid, DingRtcVideoTrack videoTrack, int timeCost);
    static void OnFirstAudioPacketSend(DingRtcEventHandler *handler, int timeCost);
    static void OnFirstAudioPacketReceived(DingRtcEventHandler *handler, const char* uid, int timeCost);
    static void OnAudioSubscribeStateChanged(DingRtcEventHandler *handler, const char* uid, DingRtcSubscribeState oldState, DingRtcSubscribeState newState, int elapseSinceLastState, const char* channel);
    static void OnVideoSubscribeStateChanged(DingRtcEventHandler *handler, const char* uid, DingRtcSubscribeState oldState, DingRtcSubscribeState newState, int elapseSinceLastState, const char* channel);
    static void OnScreenShareSubscribeStateChanged(DingRtcEventHandler *handler, const char* uid, DingRtcSubscribeState oldState, DingRtcSubscribeState newState, int elapseSinceLastState, const char* channel);
    static void OnSubscribeStreamTypeChanged(DingRtcEventHandler *handler, const char* uid, DingRtcVideoStreamType oldStreamType, DingRtcVideoStreamType newStreamType, int elapseSinceLastState, const char* channel);
    static void OnRecordingDeviceAudioLevel(DingRtcEventHandler *handler, int level);
    static void OnPlayoutDeviceAudioLevel(DingRtcEventHandler *handler, int level);
    static void OnAudioVolumeIndication(DingRtcEventHandler *handler, const DingRtcAudioVolumeInfo* speakers, unsigned int speakerNumber);
    static void OnAudioDeviceStateChanged(DingRtcEventHandler *handler, const char* deviceId, DingRtcDeviceType deviceType, DingRtcDeviceState deviceState);
    static void OnApiCalledExecuted(DingRtcEventHandler *handler, int error, const char* api, const char* result);
    static void OnSnapshotComplete(DingRtcEventHandler *handler, const char* userId, DingRtcVideoTrack videoTrack, const char* filePath, int width, int height, bool success);

    // --- DingRtcAudioFrameObserver
    static void OnPlaybackAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame);
    static void OnCapturedAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame);
    static void OnProcessCapturedAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame);
    static void OnPublishAudioFrame(DingRtcAudioFrameObserver* observer, DingRtcAudioFrame* frame);
    static void OnRemoteUserAudioFrame(DingRtcAudioFrameObserver* observer, const char *uid, DingRtcAudioFrame* frame);

    void OnPlaybackAudioFrame(DingRtcAudioFrame* frame);
    void OnCapturedAudioFrame(DingRtcAudioFrame* frame);
    void OnProcessCapturedAudioFrame(DingRtcAudioFrame* frame);
    void OnPublishAudioFrame(DingRtcAudioFrame* frame);
    void OnRemoteUserAudioFrame(const char *uid, DingRtcAudioFrame* frame);

private:
    DingRtcHandle rtc_handle_ = nullptr;
    DingRtcAuthInfo auth_info_;
    DingRtcEventHandler event_handler_;
    DingRtcAudioFrameObserver audio_frame_observer_;

    AppConfig app_config_;

    int64_t join_leave_count_{0};
    std::unique_ptr<tlog::Thread> thread_;
    
    std::string audio_dump_dir_;
    std::string video_dump_dir_;
    FILE *playback_audio_dump_file_{nullptr};
    FILE *remote_video_dump_file_{nullptr};

    std::map<std::string, FILE *> remote_user_audio_files_;

    std::unique_ptr<ExternalSourcePusherC> external_source_pusher_;
};

#endif // __CAPI_TEST_RUNNER_H__
