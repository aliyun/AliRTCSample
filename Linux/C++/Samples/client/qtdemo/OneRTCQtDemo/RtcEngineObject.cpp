#include "RtcEngineObject.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "AppSettings.h"
#include "Utils.h"

#define CHECK_ENGINE_INITIALIZED(res)                        \
    if (!engine_) {                                          \
        qDebug("%s, engine not initialized!", __FUNCTION__); \
        return res;                                          \
    }

using namespace ding::rtc;

RtcEngineObject *RtcEngineObject::instance()
{
    static RtcEngineObject *g_instance = new RtcEngineObject();
    return g_instance;
}

void RtcEngineObject::registerRtcEngineTypes()
{
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineStats");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineUserOfflineReason");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineOnByeType");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineConnectionStatus");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineConnectionStatusChangeReason");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineNetworkQuality");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::QualityReport");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::LocalAudioStats");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::LocalVideoStats");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEnginePublishState");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineSubscribeState");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineAudioTrack");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineVideoTrack");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::AudioVolumeInfo");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineDeviceType");
    qRegisterMetaType<ding::rtc::RtcEngineStats>("ding::rtc::RtcEngineDeviceState");
}

RtcEngine *RtcEngineObject::getEngine(const QString &extras)
{
    if (!engine_) {
        engine_ = RtcEngine::Create(extras.toUtf8().constData());
        engine_->SetEngineEventListener(this);
    }
    return engine_;
}

void RtcEngineObject::destroyEngine()
{
    if (engine_) {
        RtcEngine::Destroy(engine_);
        engine_ = nullptr;
    }
}

int RtcEngineObject::publishLocalAudioStream(bool pubAudio)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->PublishLocalAudioStream(pubAudio);
}

int RtcEngineObject::publishLocalVideoStream(bool pubVideo)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->PublishLocalVideoStream(pubVideo);
}

int RtcEngineObject::subscribeAllRemoteAudio(bool sub)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->SubscribeAllRemoteAudioStreams(sub);
}

int RtcEngineObject::subscribeAllRemoteVideo(bool sub)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->SubscribeAllRemoteVideoStreams(sub);
}

int RtcEngineObject::subscribeRemoteAudio(const QString &usreId, bool sub)
{
    CHECK_ENGINE_INITIALIZED(-1);
    // TODO: sdk not support yet
    // return engine_->SubscribeRemoteAudioStream(userId.toUtf8().constData(), sub);
    return 0;
}

int RtcEngineObject::subscribeRemoteVideo(const QString &userId, RtcEngineVideoTrack track, bool sub)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->SubscribeRemoteVideoStream(userId.toUtf8().constData(), track, sub);
}

int RtcEngineObject::setRemoteVideoStreamType(const QString &userId, RtcEngineVideoStreamType streamType)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->SetRemoteVideoStreamType(userId.toUtf8().constData(), streamType);
}

int RtcEngineObject::joinChannel(const QString &channelName, const QString &userId, const QString &userName)
{
    CHECK_ENGINE_INITIALIZED(-1);
    RtcEngineAuthInfo authInfo;
    QString appId = AppSettings::instance()->getAppId();
    QString appEnv = AppSettings::instance()->getAppEnv();
    QString passwd = AppSettings::instance()->getAuthPassword();
    int res = RequestAuthInfo(appId, userId, userName, channelName, appEnv, passwd, authInfo);
    return res;
}

int RtcEngineObject::leaveChannel()
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->LeaveChannel();
}

int RtcEngineObject::muteLocalAudio(bool muted)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->MuteLocalAudio(muted);
}

int RtcEngineObject::muteLocalVideo(bool muted)
{
    CHECK_ENGINE_INITIALIZED(-1);
    return engine_->MuteLocalVideo(muted, RtcEngineVideoTrackCamera);
}

int RtcEngineObject::localVideoPreview(void *window, bool previewOn, RtcEngineRenderMode renderMode)
{
    qDebug("RtcEngineObject::localVideoPreview, window: %p", window);
    CHECK_ENGINE_INITIALIZED(-1);

    int res = 0;
    if (previewOn || !window) {
        RtcEngineVideoCanvas canvas;
        canvas.displayView = window;
        canvas.renderMode = renderMode;
        res = engine_->StartPreview();
        res = engine_->SetLocalViewConfig(canvas, RtcEngineVideoTrackCamera);
    } else {
        res = engine_->StopPreview();
    }
    return res;
}

int RtcEngineObject::remoteVideoRender(const QString &userId, void *window, RtcEngineRenderMode renderMode)
{
    CHECK_ENGINE_INITIALIZED(-1);

    RtcEngineVideoCanvas canvas;
    canvas.displayView = window;
    canvas.renderMode = renderMode;
    int res = engine_->SetRemoteViewConfig(canvas, userId.toUtf8().constData(), RtcEngineVideoTrackCamera);
    return res;
}

static QList<DeviceInfo> fromRtcEngineDeviceInfoList(RtcEngineDeviceInfoList *deviceList)
{
    QList<DeviceInfo> devices;
    int deviceCount = deviceList ? deviceList->GetCount() : 0;
    for (int i = 0; i < deviceCount; i++) {
        RtcEngineDeviceInfo deviceInfo = deviceList->GetDeviceInfo(i);
        DeviceInfo device;
        device.deviceName = deviceInfo.deviceName.c_str();
        device.deviceId = deviceInfo.deviceID.c_str();
        devices.push_back(device);
    }
    return devices;
}

QList<DeviceInfo> RtcEngineObject::getCameraDevices()
{
    QList<DeviceInfo> devices;
    if (engine_) {
        RtcEngineDeviceInfoList *deviceList = engine_->GetVideoDeviceManager()->GetCameraList();
        devices = fromRtcEngineDeviceInfoList(deviceList);
    }
    return devices;
}

QList<DeviceInfo> RtcEngineObject::getMicrophoneDevices()
{
    QList<DeviceInfo> devices;
    if (engine_) {
        RtcEngineDeviceInfoList *deviceList = engine_->GetAudioDeviceManager()->GetRecordingDeviceList();
        devices = fromRtcEngineDeviceInfoList(deviceList);
    }
    return devices;
}

QList<DeviceInfo> RtcEngineObject::getSpeakerDevices()
{
    QList<DeviceInfo> devices;
    if (engine_) {
        RtcEngineDeviceInfoList *deviceList = engine_->GetAudioDeviceManager()->GetPlayoutDeviceList();
        devices = fromRtcEngineDeviceInfoList(deviceList);
    }
    return devices;
}

DeviceInfo RtcEngineObject::getCurrentCamera()
{
    DeviceInfo device;
    if (engine_) {
        device.deviceName = engine_->GetVideoDeviceManager()->GetCurrentCameraName().c_str();
        device.deviceId = engine_->GetVideoDeviceManager()->GetCurrentCameraID().c_str();
    }
    return device;
}

static int findDeviceInfoById(QList<DeviceInfo> devices, const QString &deviceId, DeviceInfo &found)
{
    for (int i = 0; i < devices.size(); i++) {
        if (devices[i].deviceId == deviceId) {
            found = devices[i];
            return i;
        }
    }
    return -1;
}

DeviceInfo RtcEngineObject::getCurrentMicrophone()
{
    DeviceInfo device;
    if (engine_) {
        device.deviceId = engine_->GetAudioDeviceManager()->GetCurrentRecordingDeviceId().c_str();
        QList<DeviceInfo> devices = getMicrophoneDevices();
        findDeviceInfoById(devices, device.deviceId, device);
    }
    return device;
}

DeviceInfo RtcEngineObject::getCurrentSpeaker()
{
    DeviceInfo device;
    if (engine_) {
        device.deviceId = engine_->GetAudioDeviceManager()->GetCurrentPlayoutDeviceId().c_str();
        QList<DeviceInfo> devices = getSpeakerDevices();
        findDeviceInfoById(devices, device.deviceId, device);
    }
    return device;
}

void RtcEngineObject::setCurrentCamera(const QString &deviceName)
{
    if (engine_) {
        engine_->GetVideoDeviceManager()->SetCurrentCameraName(deviceName.toUtf8().constData());
    }
}

void RtcEngineObject::setCurrentMicrophone(const QString &deviceId)
{
    if (engine_) {
        engine_->GetAudioDeviceManager()->SetCurrentRecordingDeviceId(deviceId.toUtf8().constData());
    }
}

void RtcEngineObject::setCurrentSpeaker(const QString &deviceId)
{
    if (engine_) {
        engine_->GetAudioDeviceManager()->SetCurrentPlayoutDeviceId(deviceId.toUtf8().constData());
    }
}

void RtcEngineObject::onAuthInfoResponse(QString response)
{
    qDebug("[%d] onAuthInfoResponse, response:%s\n", CurrentThreadId(), qPrintable(response));

    QJsonDocument document = QJsonDocument::fromJson(response.toUtf8());
    if (document.isObject()) {
        QJsonObject j_resp = document.object();
        int code = j_resp["code"].toInt();
        QJsonObject j_data = j_resp["data"].toObject();
        if (!j_data.isEmpty()) {
            std::string appId = j_data["appid"].toString().toUtf8().constData();
            std::string userId = j_data["userid"].toString().toUtf8().constData();
            // std::string nonce = j_data["nonce"].toString().toUtf8().constData();
            // uint64_t timestamp = static_cast<uint64_t>(j_data["timestamp"].toDouble());
            std::string token = j_data["token"].toString().toUtf8().constData();

            std::string channelName = AppSettings::instance()->getChannelName().toUtf8().constData();
            std::string userName = AppSettings::instance()->getUserName().toUtf8().constData();

            std::string gslbServer;
            QJsonArray gslbs = j_data["gslb"].toArray();
            if (gslbs.size() > 0) {
                gslbServer = gslbs[0].toString().toUtf8().constData();
            }

            // RtcEngineAuthInfo should define a struct to wrap string
            RtcEngineAuthInfo authInfo;
            authInfo.appId = appId.data();
            authInfo.channelId = channelName.data();
            authInfo.userId = userId.data();
            // authInfo.nonce = nonce.data();
            authInfo.token = token.data();
            // authInfo.timestamp = timestamp;
            authInfo.gslbServer = gslbServer.data();

            engine_->JoinChannel(authInfo, userName.data());
        }
    }
}

void RtcEngineObject::onAuthInfoError(int statusCode, QString desc)
{
    qDebug("[%d] onAuthInfoError, statusCode:%d, desc:%s\n", CurrentThreadId(), statusCode, qPrintable(desc));
}

RtcEngineObject::RtcEngineObject(QObject *parent)
    : QObject(parent)
    , engine_(nullptr)
{
    connect(&httpClient_, &HttpClient::onResponse, this, &RtcEngineObject::onAuthInfoResponse);
    connect(&httpClient_, &HttpClient::onError, this, &RtcEngineObject::onAuthInfoError);
}

RtcEngineObject::~RtcEngineObject()
{
    destroyEngine();
}

int RtcEngineObject::RequestAuthInfo(const QString &appId,
                                     const QString &userId,
                                     const QString &userName,
                                     const QString &channelName,
                                     const QString &env,
                                     const QString &passwd,
                                     RtcEngineAuthInfo &authInfo)
{
    QString url;
    if (env == "onertcPre") {
        url = "https://pre-onertc-demo-app-server.dingtalk.com/login";
    } else {
        url = "https://onertc-demo-app-server.dingtalk.com/login";
    }
    QMap<QString, QString> headers;
    QMap<QString, QString> params;
    params["passwd"] = passwd;
    params["appid"] = appId;
    params["userid"] = userId;
    params["user"] = userName;
    params["room"] = channelName;
    params["env"] = env;
    params["tokensid"] = "false";

    httpClient_.get(url, headers, params);

    return 0;
}

// --- implements RtcEngineEventListener ---
void RtcEngineObject::OnJoinChannelResult(int result, const char *channel, const char *uid, int elapsed)
{
    emit onJoinChannelResult(result, channel, uid, elapsed);
}

void RtcEngineObject::OnLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats)
{
    emit onLeaveChannelResult(result, stats);
}

void RtcEngineObject::OnChannelRemainingTimeNotify(int remainingTimeInSec)
{
    emit onChannelRemainingTimeNotify(remainingTimeInSec);
}

void RtcEngineObject::OnRemoteUserOnLineNotify(const char *uid, int elapsed)
{
    emit onRemoteUserOnLineNotify(uid, elapsed);
}

void RtcEngineObject::OnRemoteUserOffLineNotify(const char *uid, ding::rtc::RtcEngineUserOfflineReason reason)
{
    emit onRemoteUserOffLineNotify(uid, reason);
}

void RtcEngineObject::OnBye(ding::rtc::RtcEngineOnByeType code)
{
    emit onBye(code);
}

void RtcEngineObject::OnConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status, ding::rtc::RtcEngineConnectionStatusChangeReason reason)
{
    emit onConnectionStatusChanged(status, reason);
}

void RtcEngineObject::OnNetworkQualityChanged(const char *uid, ding::rtc::RtcEngineNetworkQuality upQuality, ding::rtc::RtcEngineNetworkQuality downQuality)
{
    emit onNetworkQualityChanged(uid, upQuality, downQuality);
}

void RtcEngineObject::OnStatisticsLog(ding::rtc::String const &log)
{
    emit onStatisticsLog(log.c_str());
}

void RtcEngineObject::OnOccurWarning(int warn, const char *msg)
{
    emit onOccurWarning(warn, msg);
}

void RtcEngineObject::OnOccurError(int error, const char *msg)
{
    emit onOccurError(error, msg);
}

void RtcEngineObject::OnStatisticsLevelLog(int level, const ding::rtc::String &log)
{
    // emit onStatisticsLevelLog(level, log.c_str());
}

void RtcEngineObject::OnQualityReport(const ding::rtc::QualityReport &quality)
{
    // emit onQualityReport(quality);
}

void RtcEngineObject::OnLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats)
{
    // emit onLocalAudioStats(localAudioStats);
}

void RtcEngineObject::OnLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats)
{
    // emit onLocalVideoStats(localVideoStats);
}

void RtcEngineObject::OnRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats)
{
    // emit onRemoteAudioStats(remoteAudioStats);
}

void RtcEngineObject::OnRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats)
{
    // emit onRemoteVideoStats(remoteVideoStats);
}

void RtcEngineObject::OnStats(const ding::rtc::RtcEngineStats &stats)
{
    // emit onStats(stats);
}

void RtcEngineObject::OnAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel)
{
    emit onAudioPublishStateChanged(oldState, newState, elapseSinceLastState, channel);
}

void RtcEngineObject::OnVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel)
{
    emit onVideoPublishStateChanged(oldState, newState, elapseSinceLastState, channel);
}

void RtcEngineObject::OnScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, const char *channel)
{
    emit onScreenSharePublishStateChanged(oldState, newState, elapseSinceLastState, channel);
}

void RtcEngineObject::OnRemoteTrackAvailableNotify(const char *uid, ding::rtc::RtcEngineAudioTrack audioTrack, ding::rtc::RtcEngineVideoTrack videoTrack)
{
    emit onRemoteTrackAvailableNotify(uid, audioTrack, videoTrack);
}

void RtcEngineObject::OnUserAudioMuted(const char *uid, bool isMute)
{
    emit onUserAudioMuted(uid, isMute);
}

void RtcEngineObject::OnUserVideoMuted(const char *uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack)
{
    emit onUserVideoMuted(uid, isMute, videoTrack);
}

void RtcEngineObject::OnFirstRemoteVideoFrameDrawn(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int width, int height, int elapsed)
{
    emit onFirstRemoteVideoFrameDrawn(uid, videoTrack, width, height, elapsed);
}

void RtcEngineObject::OnFirstLocalVideoFrameDrawn(int width, int height, int elapsed)
{
    emit onFirstLocalVideoFrameDrawn(width, height, elapsed);
}

void RtcEngineObject::OnFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost)
{
    emit onFirstVideoPacketSend(videoTrack, timeCost);
}

void RtcEngineObject::OnFirstVideoPacketReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost)
{
    emit onFirstVideoPacketReceived(uid, videoTrack, timeCost);
}

void RtcEngineObject::OnFirstVideoFrameReceived(const char *uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost)
{
    emit onFirstVideoFrameReceived(uid, videoTrack, timeCost);
}

void RtcEngineObject::OnFirstAudioPacketSend(int timeCost)
{
    emit onFirstAudioPacketSend(timeCost);
}

void RtcEngineObject::OnFirstAudioPacketReceived(const char *uid, int timeCost)
{
    emit onFirstAudioPacketReceived(uid, timeCost);
}

void RtcEngineObject::OnAudioSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel)
{
    emit onAudioSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
}

void RtcEngineObject::OnVideoSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel)
{
    emit onVideoSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
}

void RtcEngineObject::OnScreenShareSubscribeStateChanged(const char *uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, const char *channel)
{
    emit onScreenShareSubscribeStateChanged(uid, oldState, newState, elapseSinceLastState, channel);
}

void RtcEngineObject::OnSubscribeStreamTypeChanged(const char *uid, ding::rtc::RtcEngineVideoStreamType oldStreamType, ding::rtc::RtcEngineVideoStreamType newStreamType, int elapseSinceLastState, const char *channel)
{
    emit onSubscribeStreamTypeChanged(uid, oldStreamType, newStreamType, elapseSinceLastState, channel);
}

void RtcEngineObject::OnRecordingDeviceAudioLevel(int level)
{
    emit onRecordingDeviceAudioLevel(level);
}

void RtcEngineObject::OnPlayoutDeviceAudioLevel(int level)
{
    emit onPlayoutDeviceAudioLevel(level);
}

void RtcEngineObject::OnAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers, unsigned int speakerNumber)
{
    emit onAudioVolumeIndication(speakers, speakerNumber);
}

void RtcEngineObject::OnAudioDeviceStateChanged(const char *deviceId, ding::rtc::RtcEngineDeviceType deviceType, ding::rtc::RtcEngineDeviceState deviceState)
{
    emit onAudioDeviceStateChanged(deviceId, deviceType, deviceState);
}

void RtcEngineObject::OnApiCalledExecuted(int error, const char *api, const char *result)
{

}

// --- implements RtcEngineAudioFrameObserver ---
//void RtcEngineObject::OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) {}
//void RtcEngineObject::OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) {}
//void RtcEngineObject::OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) {}
//void RtcEngineObject::OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) {}

// --- implements RtcEngineVideoFrameObserver ---
//ding::rtc::RtcEngineVideoPixelFormat RtcEngineObject::GetVideoFormatPreference()
//{
//    return RtcEngineVideoI420;
//}
//bool RtcEngineObject::OnCaptureVideoFrame(ding::rtc::RtcEngineVideoFrame &frame)
//{
//    return true;
//}
//bool RtcEngineObject::OnRemoteVideoFrame(ding::rtc::String uid, ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame)
//{
//    return true;
//}
//bool RtcEngineObject::OnPreEncodeVideoFrame(ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame)
//{
//    return true;
//}
