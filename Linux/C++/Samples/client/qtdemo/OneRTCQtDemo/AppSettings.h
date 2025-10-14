#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QString>
#include <QSize>
#include <memory>

#include "DeviceInfo.h"

class AppSettings
{
public:
    static AppSettings *instance();

    void setAppId(const QString &appId);
    QString getAppId();

    void setAppEnv(const QString &env);
    QString getAppEnv();

    void setAuthPassword(const QString &passwd);
    QString getAuthPassword();

    void setChannelName(const QString &channelName);
    QString getChannelName();

    void setUserId(const QString &userId);
    QString getUserId();

    void setUserName(const QString &userName);
    QString getUserName();

    void enablePublishAudio(bool enabled);
    bool isPublishAudioEnabled();

    void enablePublishVideo(bool enabled);
    bool isPublishVideoEnabled();

    void enableSubscribeAllAudio(bool enabled);
    bool isSubscirbeAllAudioEnabled();

    void enableSubscribeAllVideo(bool enabled);
    bool isSubscirbeAllVideoEnabled();

    void enableLocalPreviewMirror(bool enabled);
    bool isLocalPreviewMirrorEnabled();

    void enablePublishVideoMirror(bool enabled);
    bool isPublishVideoMirrorEnabled();

    void setVideoRenderMode(int mode);
    int getVideoRenderMode();

    void setVideoEncoderDimension(int width, int height);
    void getVideoEncoderDimension(int &width, int &height);

    void setVideoEncoderFps(int fps);
    int getVideoEncoderFps();

    void setSelectedCamera(const QString &deviceName);
    QString getSelectedCamera();

    void setSelectedMic(const QString &deviceName);
    QString getSelectedMic();

    void setSelectedSpeaker(const QString &deviceName);
    QString getSelectedSpeaker();

    void enableExternalVideoSource(bool enabled);
    bool isExternalVideoSourceEnabled();

    void setExternalVideoSourceFilePath(const QString &filePath);
    QString getExternalVideoSourceFilePath();

    void setExternalVideoSourceDimension(int width, int height);
    void getExternalVideoSourceDimension(int &width, int &height);

    void enableExternalScreenSource(bool enabled);
    bool isExternalScreenSourceEnabled();

    void setExternalScreenSourceFilePath(const QString &filePath);
    QString getExternalScreenSourceFilePath();

    void setExternalScreenSourceDimension(int width, int height);
    void getExternalScreenSourceDimension(int &width, int &height);

    void enableExternalAudioSource(bool enabled);
    bool isExternalAudioSourceEnabled();

    void setExternalAudioSourceFilePath(const QString &filePath);
    QString getExternalAudioSourceFilePath();

    void setExternalAudioSourceConfiguration(int samplerate, int channels);
    void getExternalAudioSourceConfiguration(int &samplerate, int &channels);

    void enableExternalAudioPlayout(bool enabled);
    bool isExternalAudioPlayoutEnabled();

    void setExternalAudioPlayoutFilePath(const QString &filePath);
    QString getExternalAudioPlayoutFilePath();

    void setExternalAudioPlayoutConfiguration(int samplerate, int channels);
    void getExternalAudioPlayoutConfiguration(int &samplerate, int &channels);

private:
    AppSettings();
    ~AppSettings();

private:
    std::unique_ptr<QSettings> settings_;
};

#endif  // APPSETTINGS_H
