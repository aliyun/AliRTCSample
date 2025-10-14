#ifndef RTCENGINEOBJECT_H
#define RTCENGINEOBJECT_H

#include <QObject>
#include <memory>

#include "DeviceInfo.h"
#include "engine_interface.h"
#include "HttpClient.h"

//for QMetaType
Q_DECLARE_METATYPE(ding::rtc::RtcEngineStats)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineUserOfflineReason)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineOnByeType)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineConnectionStatus)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineConnectionStatusChangeReason)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineNetworkQuality)
Q_DECLARE_METATYPE(ding::rtc::QualityReport)
Q_DECLARE_METATYPE(ding::rtc::LocalAudioStats)
Q_DECLARE_METATYPE(ding::rtc::LocalVideoStats)
Q_DECLARE_METATYPE(ding::rtc::RtcEnginePublishState)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineSubscribeState)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineAudioTrack)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineVideoTrack)
Q_DECLARE_METATYPE(ding::rtc::AudioVolumeInfo)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineDeviceType)
Q_DECLARE_METATYPE(ding::rtc::RtcEngineDeviceState)


class RtcEngineObject
    : public QObject
    , public ding::rtc::RtcEngineEventListener
{
    Q_OBJECT
public:
    static RtcEngineObject *instance();

    static void registerRtcEngineTypes();

    ding::rtc::RtcEngine *getEngine(const QString &extra = "");
    void destroyEngine();

    int publishLocalAudioStream(bool pubAudio);
    int publishLocalVideoStream(bool pubVideo);
    int subscribeAllRemoteAudio(bool sub);
    int subscribeAllRemoteVideo(bool sub);
    int subscribeRemoteAudio(const QString &userId, bool sub);
    int subscribeRemoteVideo(const QString &userId, ding::rtc::RtcEngineVideoTrack track, bool sub);
    int setRemoteVideoStreamType(const QString &userId, ding::rtc::RtcEngineVideoStreamType streamType);

    int joinChannel(const QString &channelName, const QString &userId, const QString &userName);
    int leaveChannel();

    int muteLocalAudio(bool muted);
    int muteLocalVideo(bool muted);

    int localVideoPreview(void *window, bool previewOn, ding::rtc::RtcEngineRenderMode renderMode);
    int remoteVideoRender(const QString &userId, void *window, ding::rtc::RtcEngineRenderMode renderMode);

    QList<DeviceInfo> getCameraDevices();
    QList<DeviceInfo> getMicrophoneDevices();
    QList<DeviceInfo> getSpeakerDevices();

    DeviceInfo getCurrentCamera();
    DeviceInfo getCurrentMicrophone();
    DeviceInfo getCurrentSpeaker();

    void setCurrentCamera(const QString &deviceName);
    void setCurrentMicrophone(const QString &deviceId);
    void setCurrentSpeaker(const QString &deviceId);

signals:
    void onJoinChannelResult(int result, QString channel, QString userId, int elapsed);
    void onLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats);
    void onChannelRemainingTimeNotify(int remainingTimeInSec);
    void onRemoteUserOnLineNotify(QString userId, int elapsed);
    void onRemoteUserOffLineNotify(QString userId, ding::rtc::RtcEngineUserOfflineReason reason);
    void onBye(ding::rtc::RtcEngineOnByeType code);
    void onConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status, ding::rtc::RtcEngineConnectionStatusChangeReason reason);
    void onNetworkQualityChanged(QString userId, ding::rtc::RtcEngineNetworkQuality upQuality, ding::rtc::RtcEngineNetworkQuality downQuality);
    void onStatisticsLog(QString log);
    void onOccurWarning(int warn, QString msg);
    void onOccurError(int error, QString msg);
    void onStatisticsLevelLog(int level, QString log);
    void onQualityReport(const ding::rtc::QualityReport &quality);
    void onLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats);
    void onLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats);
    void onRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats);
    void onRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats);
    void onStats(const ding::rtc::RtcEngineStats &stats);
    void onAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, QString channel);
    void onVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, QString channel);
    void onScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, QString channel);
    void onRemoteTrackAvailableNotify(QString userId, ding::rtc::RtcEngineAudioTrack audioTrack, ding::rtc::RtcEngineVideoTrack videoTrack);
    void onUserAudioMuted(QString userId, bool isMute);
    void onUserVideoMuted(QString userId, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack);
    void onFirstRemoteVideoFrameDrawn(QString userId, ding::rtc::RtcEngineVideoTrack videoTrack, int width, int height, int elapsed);
    void onFirstLocalVideoFrameDrawn(int width, int height, int elapsed);
    void onFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost);
    void onFirstVideoPacketReceived(QString userId, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost);
    void onFirstVideoFrameReceived(QString userId, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost);
    void onFirstAudioPacketSend(int timeCost);
    void onFirstAudioPacketReceived(QString userId, int timeCost);
    void onAudioSubscribeStateChanged(QString userId, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, QString channel);
    void onVideoSubscribeStateChanged(QString userId, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, QString channel);
    void onScreenShareSubscribeStateChanged(QString userId, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, QString channel);
    void onSubscribeStreamTypeChanged(QString userId, ding::rtc::RtcEngineVideoStreamType oldStreamType, ding::rtc::RtcEngineVideoStreamType newStreamType, int elapseSinceLastState, QString channel);
    void onRecordingDeviceAudioLevel(int level);
    void onPlayoutDeviceAudioLevel(int level);
    void onAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers, unsigned int speakerNumber);
    void onAudioDeviceStateChanged(QString deviceId, ding::rtc::RtcEngineDeviceType deviceType, ding::rtc::RtcEngineDeviceState deviceState);

private slots:
    void onAuthInfoResponse(QString response);
    void onAuthInfoError(int statusCode, QString desc);

private:
    explicit RtcEngineObject(QObject *parent = nullptr);
    ~RtcEngineObject();

    int RequestAuthInfo(const QString &appId,
                        const QString &userId,
                        const QString &userName,
                        const QString &channelName,
                        const QString &env,
                        const QString &passwd,
                        ding::rtc::RtcEngineAuthInfo &authInfo);

    // --- implements RtcEngineEventListener ---
    void OnJoinChannelResult(int result, const char *channel, const char *uid, int elapsed) override;
    void OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats) override;
    void OnChannelRemainingTimeNotify(int remainingTimeInSec) override;
    void OnRemoteUserOnLineNotify(const char *uid, int elapsed) override;
    void OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason) override;
    void OnBye(ding::rtc::RtcEngineOnByeType code) override;
    void OnConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status, ding::rtc::RtcEngineConnectionStatusChangeReason reason) override;
    void OnNetworkQualityChanged(const char *uid, ding::rtc::RtcEngineNetworkQuality upQuality, ding::rtc::RtcEngineNetworkQuality downQuality) override;
    void OnStatisticsLog(ding::rtc::String const &log) override;
    void OnOccurWarning(int warn, const char *msg) override;
    void OnOccurError(int error, const char *msg) override;
    void OnStatisticsLevelLog(int level, const ding::rtc::String &log) override;
    void OnQualityReport(const ding::rtc::QualityReport &quality) override;
    void OnLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats) override;
    void OnLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats) override;
    void OnRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats) override;
    void OnRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats) override;
    void OnStats(const ding::rtc::RtcEngineStats &stats) override;
    void OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel) override;
    void OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel) override;
    void OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel) override;
    void OnRemoteTrackAvailableNotify(const char *uid, ding::rtc::RtcEngineAudioTrack audioTrack, ding::rtc::RtcEngineVideoTrack videoTrack) override;
    void OnUserAudioMuted(const char *uid, bool isMute) override;
    void OnUserVideoMuted(const char *uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack) override;
    void OnFirstRemoteVideoFrameDrawn(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int width, int height, int elapsed) override;
    void OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed) override;
    void OnFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) override;
    void OnFirstVideoPacketReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) override;
    void OnFirstVideoFrameReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost) override;
    void OnFirstAudioPacketSend(int timeCost) override;
    void OnFirstAudioPacketReceived(const char *uid, int timeCost) override;
    void OnAudioSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel) override;
    void OnVideoSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel) override;
    void OnScreenShareSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel) override;
    void OnSubscribeStreamTypeChanged(const char *uid, ding::rtc::RtcEngineVideoStreamType oldStreamType, ding::rtc::RtcEngineVideoStreamType newStreamType, int elapseSinceLastState, const char *channel) override;
    void OnRecordingDeviceAudioLevel(int level) override;
    void OnPlayoutDeviceAudioLevel(int level) override;
    void OnAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers, unsigned int speakerNumber) override;
    void OnAudioDeviceStateChanged(const char *deviceId, ding::rtc::RtcEngineDeviceType deviceType, ding::rtc::RtcEngineDeviceState deviceState) override;
    void OnApiCalledExecuted(int error, const char *api, const char *result) override;

    // --- implements RtcEngineAudioFrameObserver ---
//    void OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;
//    void OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;
//    void OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;
//    void OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;

    // --- implements RtcEngineVideoFrameObserver ---
//    ding::rtc::RtcEngineVideoPixelFormat GetVideoFormatPreference() override;
//    bool OnCaptureVideoFrame(ding::rtc::RtcEngineVideoFrame &frame) override;
//    bool OnRemoteVideoFrame(ding::rtc::String uid, ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override;
//    bool OnPreEncodeVideoFrame(ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override;

private:
    ding::rtc::RtcEngine *engine_;
    // std::unique_ptr<RtcEngineEventListener> eventHandler_;
    HttpClient httpClient_;
};

#endif  // RTCENGINEOBJECT_H
