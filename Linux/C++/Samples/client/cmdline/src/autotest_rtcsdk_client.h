#ifndef __AUTOTEST_RTCSDK_CLIENT_H__
#define __AUTOTEST_RTCSDK_CLIENT_H__

#include <string>

#include "external_source_pusher.h"
#include "engine_interface.h"
#include "nlohmann/json.hpp"

class AutoTestTcpClient;

// ----------- AutoTestRtcSdkClient ----------
class AutoTestRtcSdkClient : public ding::rtc::RtcEngineEventListener {
public:
    AutoTestRtcSdkClient(AutoTestTcpClient *tcp_client);
    ~AutoTestRtcSdkClient();

    void SetRtcLogDir(const std::string &log_dir) { rtc_log_dir_ = log_dir; }

    std::string OnRecvCommand(uint64_t invoke_id, const std::string &cmd);

    // --- implements RtcEngineEventListener ---
    virtual void OnJoinChannelResult(int result, const char *channel, const char *userId, int elapsed);
    virtual void OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats);
    virtual void OnChannelRemainingTimeNotify(int remainingTimeInSec);
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

private:
    AutoTestTcpClient *tcp_client_{nullptr};

    ding::rtc::RtcEngine *rtc_engine_{nullptr};
    ding::rtc::RtcEngineVideoCanvas camera_canvas_, screen_canvas_;
    std::string rtc_log_dir_;
    std::unique_ptr<ExternalSourcePusher> external_source_pusher_;

    typedef enum {
        kStatsUnknown = 0,
        kRtcEngineStats = 1,
        kLocalAudioStats = 2,
        kRemoteAudioStats = 3,
        kLocalVideoStats = 4,
        kRemoteVideoStats = 5
    } RtcStatsType;

    uint64_t send_stats_flag_{0};
    inline bool get_flag(const uint64_t &flag, int pos) {
        return (1ULL << pos) & flag;
    }
    inline void set_flag(uint64_t &flag, int pos, bool enable) {
        if (enable) {
            flag |= (uint64_t)(1ULL << pos);
        } else {
            flag &= (uint64_t) ~(1ULL << pos);
        }
    }

    inline bool is_static_method(const std::string &method) {
        return method == "create" || method == "destroy" || method == "set_log_dir_path" ||
               method == "get_sdk_version" || method == "set_log_level" || method == "get_error_description";
    }

    void DestroyRtcEngine();
    void SendEventToServer(const char *method, const nlohmann::json &j_params);
    std::string GetResourceDir();
};

#endif  // __AUTOTEST_RTCSDK_CLIENT_H__
