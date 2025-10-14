#include "AppSettings.h"
#include "engine_types.h"

AppSettings *AppSettings::instance()
{
    static AppSettings instance;
    return &instance;
}

AppSettings::AppSettings() {
    settings_ = std::make_unique<QSettings>("OneRTCQtDemo.ini", QSettings::IniFormat);
}

AppSettings::~AppSettings() {

}

void AppSettings::setAppId(const QString &appId)
{
    settings_->setValue("appId", appId);
}

QString AppSettings::getAppId()
{
    return settings_->value("appId").toString();
}

void AppSettings::setAppEnv(const QString &env)
{
    settings_->setValue("appEnv", env);
}

QString AppSettings::getAppEnv()
{
    return settings_->value("appEnv").toString();
}

void AppSettings::setAuthPassword(const QString &passwd)
{
    settings_->setValue("authPassword", passwd);
}

QString AppSettings::getAuthPassword()
{
    return settings_->value("authPassword").toString();
}

void AppSettings::setChannelName(const QString &channelName)
{
    settings_->setValue("channelName", channelName);
}

QString AppSettings::getChannelName()
{
    return settings_->value("channelName").toString();
}

void AppSettings::setUserId(const QString &userId)
{
    settings_->setValue("userId", userId);
}

QString AppSettings::getUserId()
{
    return settings_->value("userId").toString();
}

void AppSettings::setUserName(const QString &userName)
{
    settings_->setValue("userName", userName);
}

QString AppSettings::getUserName()
{
    return settings_->value("userName").toString();
}

void AppSettings::enablePublishAudio(bool enabled)
{
    settings_->setValue("publishAudio", enabled);
}

bool AppSettings::isPublishAudioEnabled()
{
    return settings_->value("publishAudio", true).toBool();
}

void AppSettings::enablePublishVideo(bool enabled)
{
    settings_->setValue("publishVideo", enabled);
}

bool AppSettings::isPublishVideoEnabled()
{
    return settings_->value("publishVideo", true).toBool();
}

void AppSettings::enableSubscribeAllAudio(bool enabled)
{
    settings_->setValue("subscribeAllAudio", enabled);
}

bool AppSettings::isSubscirbeAllAudioEnabled()
{
    return settings_->value("subscribeAllAudio", true).toBool();
}

void AppSettings::enableSubscribeAllVideo(bool enabled)
{
    settings_->setValue("subscribeAllVideo", enabled);
}

bool AppSettings::isSubscirbeAllVideoEnabled()
{
    return settings_->value("subscribeAllVideo", true).toBool();
}

void AppSettings::enableLocalPreviewMirror(bool enabled)
{
    settings_->setValue("video/localPreviewMirror", enabled);
}

bool AppSettings::isLocalPreviewMirrorEnabled()
{
    return settings_->value("video/localPreviewMirror", true).toBool();
}

void AppSettings::enablePublishVideoMirror(bool enabled)
{
    settings_->setValue("video/publishVideoMirror", enabled);
}

bool AppSettings::isPublishVideoMirrorEnabled()
{
    return settings_->value("video/publishVideoMirror", false).toBool();
}

void AppSettings::setVideoRenderMode(int mode)
{
    settings_->setValue("video/renderMode", mode);
}

int AppSettings::getVideoRenderMode()
{
    return settings_->value("video/renderMode", (int)ding::rtc::RtcEngineRenderModeFill).toInt();
}

void AppSettings::setVideoEncoderDimension(int width, int height)
{
    settings_->setValue("video/encoderWidth", width);
    settings_->setValue("video/encoderHeight", height);
}

void AppSettings::getVideoEncoderDimension(int &width, int &height)
{
    width = settings_->value("video/encoderWidth", 640).toInt();
    height = settings_->value("video/encoderHeight", 480).toInt();
}

void AppSettings::setVideoEncoderFps(int fps)
{
    settings_->setValue("video/encoderFps", fps);
}

int AppSettings::getVideoEncoderFps()
{
    return settings_->value("video/encoderFps", 15).toInt();
}

void AppSettings::setSelectedCamera(const QString &deviceName)
{
    settings_->setValue("video/selectedCameraDeviceName", deviceName);
    // settings_->setValue("video/selectedCameraDeviceId", deviceInfo.deviceId);
}

QString AppSettings::getSelectedCamera()
{
    // DeviceInfo deviceInfo;
    // deviceInfo.deviceName = settings_->value("video/selectedCameraDeviceName", "").toString();
    // deviceInfo.deviceId = settings_->value("video/selectedCameraDeviceId", "").toString();
    return settings_->value("video/selectedCameraDeviceName", "").toString();
}

void AppSettings::setSelectedMic(const QString &deviceName)
{
    // settings_->setValue("audio/selectedMicDeviceName", deviceInfo.deviceName);
    // settings_->setValue("audio/selectedMicDeviceId", deviceInfo.deviceId);
    settings_->setValue("audio/selectedMicDeviceName", deviceName);
}

QString AppSettings::getSelectedMic()
{
    // DeviceInfo deviceInfo;
    // deviceInfo.deviceName = settings_->value("video/selectedMicDeviceName", "").toString();
    // deviceInfo.deviceId = settings_->value("video/selectedMicDeviceId", "").toString();
    return settings_->value("audio/selectedMicDeviceName", "").toString();
}

void AppSettings::setSelectedSpeaker(const QString &deviceName)
{
    // settings_->setValue("audio/selectedSpeakerDeviceName", deviceInfo.deviceName);
    // settings_->setValue("audio/selectedSpeakerDeviceId", deviceInfo.deviceId);
    settings_->setValue("audio/selectedSpeakerDeviceName", deviceName);
}

QString AppSettings::getSelectedSpeaker()
{
    // DeviceInfo deviceInfo;
    // deviceInfo.deviceName = settings_->value("audio/selectedSpeakerDeviceName", "").toString();
    // deviceInfo.deviceId = settings_->value("audio/selectedSpeakerDeviceId", "").toString();
    return settings_->value("audio/selectedSpeakerDeviceName", "").toString();
}

void AppSettings::enableExternalVideoSource(bool enabled)
{
    settings_->setValue("externalSource/videoSourceEnabled", enabled);
}

bool AppSettings::isExternalVideoSourceEnabled()
{
    return settings_->value("externalSource/videoSourceEnabled", false).toBool();
}

void AppSettings::setExternalVideoSourceFilePath(const QString &filePath)
{
    settings_->setValue("externalSource/videoFilePath", filePath);
}

QString AppSettings::getExternalVideoSourceFilePath()
{
    return settings_->value("externalSource/videoFilePath", "720_1280.yuv").toString();
}

void AppSettings::setExternalVideoSourceDimension(int width, int height)
{
    settings_->setValue("externalSource/videoWidth", width);
    settings_->setValue("externalSource/videoHeight", height);
}

void AppSettings::getExternalVideoSourceDimension(int &width, int &height)
{
    width = settings_->value("externalSource/videoWidth", 720).toInt();
    height = settings_->value("externalSource/videoHeight", 1280).toInt();
}

void AppSettings::enableExternalScreenSource(bool enabled)
{
    settings_->setValue("externalSource/screenSourceEnabled", enabled);
}

bool AppSettings::isExternalScreenSourceEnabled()
{
    return settings_->value("externalSource/screenSourceEnabled", false).toBool();
}

void AppSettings::setExternalScreenSourceFilePath(const QString &filePath)
{
    settings_->setValue("externalSource/screenSourceFilePath", filePath);
}

QString AppSettings::getExternalScreenSourceFilePath()
{
    return settings_->value("externalSource/screenSourceFilePath", "720_1280.yuv").toString();
}

void AppSettings::setExternalScreenSourceDimension(int width, int height)
{
    settings_->setValue("externalSource/screenSourceWidth", width);
    settings_->setValue("externalSource/screenSourceHeight", height);
}

void AppSettings::getExternalScreenSourceDimension(int &width, int &height)
{
    width = settings_->value("externalSource/screenSourceWidth", 720).toInt();
    height = settings_->value("externalSource/screenSourceHeight", 1280).toInt();
}

void AppSettings::enableExternalAudioSource(bool enabled)
{
    settings_->setValue("externalSource/audioSourceEnabled", enabled);
}

bool AppSettings::isExternalAudioSourceEnabled()
{
    return settings_->value("externalSource/audioSourceEnabled", false).toBool();
}

void AppSettings::setExternalAudioSourceFilePath(const QString &filePath)
{
    settings_->setValue("externalSource/audioSourceFilePath", filePath);
}

QString AppSettings::getExternalAudioSourceFilePath()
{
    return settings_->value("externalSource/audioSourceFilePath", "ChengDu_1ch_48000.pcm").toString();
}

void AppSettings::setExternalAudioSourceConfiguration(int samplerate, int channels)
{
    settings_->setValue("externalSource/audioSourceSamplerate", samplerate);
    settings_->setValue("externalSource/audioSourceChannels", channels);
}

void AppSettings::getExternalAudioSourceConfiguration(int &samplerate, int &channels)
{
    samplerate = settings_->value("externalSource/audioSourceSamplerate", 48000).toInt();
    channels = settings_->value("externalSource/audioSourceChannels", 1).toInt();
}

void AppSettings::enableExternalAudioPlayout(bool enabled)
{
    settings_->setValue("externalSource/audioPlayoutEnabled", enabled);
}

bool AppSettings::isExternalAudioPlayoutEnabled()
{
    return settings_->value("externalSource/audioPlayoutEnabled", false).toBool();
}

void AppSettings::setExternalAudioPlayoutFilePath(const QString &filePath)
{
    settings_->setValue("externalSource/audioPlayoutFilePath", filePath);
}

QString AppSettings::getExternalAudioPlayoutFilePath()
{
    return settings_->value("externalSource/audioSourceFilePath", "ChengDu_1ch_48000.pcm").toString();
}

void AppSettings::setExternalAudioPlayoutConfiguration(int samplerate, int channels)
{
    settings_->setValue("externalSource/audioSourceSamplerate", samplerate);
    settings_->setValue("externalSource/audioSourceChannels", channels);
}

void AppSettings::getExternalAudioPlayoutConfiguration(int &samplerate, int &channels)
{
    samplerate = settings_->value("externalSource/audioSourceSamplerate", 48000).toInt();
    channels = settings_->value("externalSource/audioSourceChannels", 1).toInt();
}


