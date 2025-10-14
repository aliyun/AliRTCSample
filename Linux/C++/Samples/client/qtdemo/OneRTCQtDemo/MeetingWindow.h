#ifndef MEETINGWINDOW_H
#define MEETINGWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>

#include "engine_types.h"
#include "engine_device_manager.h"

namespace Ui {
class MeetingWindow;
}

class CellWidget;
class SettingsWindow;

class MeetingWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MeetingWindow(QWidget *parent = nullptr);
    ~MeetingWindow();

    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent* event) override;

public:
    void addUser(QString userId);
    void removeUser(QString userId);

    void updateUsers();

private slots:
    void onScreenShareClicked();
    void onScreenShareSelected(QAction *action);

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

private:
    void updateLayout();

    CellWidget *findWidgetByUserId(QString userId);

private:
    Ui::MeetingWindow *ui;
    QGridLayout *users_grid_;
    QList<QString> users_;

//    QMenu *screen_share_menu_;

    SettingsWindow *settings_window_;
    bool isScreenShare_ = false;
    QString currentScreenShareDeviceName_;
    QPushButton *screenShareButton_;
};

#endif // MEETINGWINDOW_H
