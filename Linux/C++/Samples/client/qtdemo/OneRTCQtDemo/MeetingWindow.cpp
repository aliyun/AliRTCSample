#include "MeetingWindow.h"

#include <QGridLayout>
#include <QPushButton>
#include <QTimer>

#include "ui_MeetingWindow.h"
#include "AppSettings.h"
#include "RtcEngineObject.h"
#include "CellWidget.h"
#include "UserWidget.h"
#include "SettingsWindow.h"
#include "tlog/utils.h"

#define MAX_USER_COUNT 16

static const char *kScreenShareDesktop = "共享桌面";
static const char *kScreenShareWindow = "共享窗口";
static const char *kStartScreenShare = "共享屏幕";
static const char *kStopScreenShare = "停止共享";

using namespace ding::rtc;

MeetingWindow::MeetingWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MeetingWindow)
    , settings_window_(nullptr)
{
    ui->setupUi(this);

    setMinimumSize(1280, 720);

    QVBoxLayout *layout = new QVBoxLayout();
    QWidget *usersWidget = new QWidget(this);
    users_grid_ = new QGridLayout(usersWidget);

    QPushButton *leaveButton = new QPushButton("离会", this);
    QPushButton *settingsButton = new QPushButton(this);
    settingsButton->setIcon(QIcon(":/images/setting.png"));

    connect(leaveButton, &QPushButton::clicked, this, [this]() {
        RtcEngineObject::instance()->leaveChannel();
        close();
    });
    connect(settingsButton, &QPushButton::clicked, this, [this]() {
        if (settings_window_ == nullptr) {
            settings_window_ = new SettingsWindow(this);
        }
        settings_window_->show();
    });


    // Screen Share menu
    screenShareButton_ = new QPushButton(kStartScreenShare, this);
    connect(screenShareButton_, &QPushButton::clicked, this, &MeetingWindow::onScreenShareClicked);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(screenShareButton_);
    bottomLayout->addWidget(leaveButton);
    bottomLayout->addWidget(settingsButton);
    bottomLayout->setAlignment(Qt::AlignRight);
    bottomLayout->setContentsMargins(20, 0, 0, 0);
    bottomLayout->setSpacing(20);

    layout->addWidget(usersWidget);
    layout->addLayout(bottomLayout);
    ui->centralwidget->setLayout(layout);

    RtcEngineObject *engine = RtcEngineObject::instance();
    
#define CONNECT_RTCENGINE_SIGNALS(sig) \
    connect(engine, &RtcEngineObject::sig, this, &MeetingWindow::sig);
    
    CONNECT_RTCENGINE_SIGNALS(onJoinChannelResult);
    CONNECT_RTCENGINE_SIGNALS(onLeaveChannelResult);
    CONNECT_RTCENGINE_SIGNALS(onChannelRemainingTimeNotify);
    CONNECT_RTCENGINE_SIGNALS(onRemoteUserOnLineNotify);
    CONNECT_RTCENGINE_SIGNALS(onRemoteUserOffLineNotify);
    CONNECT_RTCENGINE_SIGNALS(onBye);
    CONNECT_RTCENGINE_SIGNALS(onConnectionStatusChanged);
    CONNECT_RTCENGINE_SIGNALS(onNetworkQualityChanged);
    CONNECT_RTCENGINE_SIGNALS(onOccurWarning);
    CONNECT_RTCENGINE_SIGNALS(onOccurError);
    CONNECT_RTCENGINE_SIGNALS(onStatisticsLevelLog);
    CONNECT_RTCENGINE_SIGNALS(onQualityReport);
    CONNECT_RTCENGINE_SIGNALS(onLocalAudioStats);
    CONNECT_RTCENGINE_SIGNALS(onLocalVideoStats);
    CONNECT_RTCENGINE_SIGNALS(onRemoteAudioStats);
    CONNECT_RTCENGINE_SIGNALS(onRemoteVideoStats);
    CONNECT_RTCENGINE_SIGNALS(onStats);
    CONNECT_RTCENGINE_SIGNALS(onAudioPublishStateChanged);
    CONNECT_RTCENGINE_SIGNALS(onVideoPublishStateChanged);
    CONNECT_RTCENGINE_SIGNALS(onScreenSharePublishStateChanged);
    CONNECT_RTCENGINE_SIGNALS(onRemoteTrackAvailableNotify);
    CONNECT_RTCENGINE_SIGNALS(onUserAudioMuted);
    CONNECT_RTCENGINE_SIGNALS(onUserVideoMuted);
    CONNECT_RTCENGINE_SIGNALS(onFirstRemoteVideoFrameDrawn);
    CONNECT_RTCENGINE_SIGNALS(onFirstLocalVideoFrameDrawn);
    CONNECT_RTCENGINE_SIGNALS(onFirstVideoPacketSend);
    CONNECT_RTCENGINE_SIGNALS(onFirstVideoPacketReceived);
    CONNECT_RTCENGINE_SIGNALS(onFirstVideoFrameReceived);
    CONNECT_RTCENGINE_SIGNALS(onFirstAudioPacketSend);
    CONNECT_RTCENGINE_SIGNALS(onFirstAudioPacketReceived);
    CONNECT_RTCENGINE_SIGNALS(onAudioSubscribeStateChanged);
    CONNECT_RTCENGINE_SIGNALS(onVideoSubscribeStateChanged);
    CONNECT_RTCENGINE_SIGNALS(onScreenShareSubscribeStateChanged);
    CONNECT_RTCENGINE_SIGNALS(onSubscribeStreamTypeChanged);
    CONNECT_RTCENGINE_SIGNALS(onRecordingDeviceAudioLevel);
    CONNECT_RTCENGINE_SIGNALS(onPlayoutDeviceAudioLevel);
    CONNECT_RTCENGINE_SIGNALS(onAudioVolumeIndication);
    CONNECT_RTCENGINE_SIGNALS(onAudioDeviceStateChanged);

    engine->publishLocalAudioStream(AppSettings::instance()->isPublishAudioEnabled());
    engine->publishLocalVideoStream(AppSettings::instance()->isPublishVideoEnabled());
    engine->subscribeAllRemoteAudio(AppSettings::instance()->isSubscirbeAllAudioEnabled());
    engine->subscribeAllRemoteVideo(AppSettings::instance()->isSubscirbeAllVideoEnabled());

    addUser("me");

//    // local preview
//    QLayoutItem* const item = users_grid_->itemAt(0);
//    if (dynamic_cast<QWidgetItem *>(item)) {
//        CellWidget *cell = (CellWidget *)item->widget();
//        RtcEngineObject::instance()->localVideoPreview((void *)cell->getRenderTarget()->winId(), true, ding::rtc::RtcEngineRenderModeFill);
//    }

    engine->joinChannel(AppSettings::instance()->getChannelName(),
                                             AppSettings::instance()->getUserId(),
                                             AppSettings::instance()->getUserName());
}

MeetingWindow::~MeetingWindow()
{
    delete ui;
}

void MeetingWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void MeetingWindow::closeEvent(QCloseEvent* event)
{
    RtcEngineObject::instance()->localVideoPreview(nullptr, true, ding::rtc::RtcEngineRenderModeFill);
    RtcEngineObject::instance()->leaveChannel();
    QWidget::closeEvent(event);
    parentWidget()->show();
}

void MeetingWindow::onJoinChannelResult(int result, QString channel, QString userId, int elapsed)
{
    
}

void MeetingWindow::onLeaveChannelResult(int result, ding::rtc::RtcEngineStats stats)
{
    
}

void MeetingWindow::onChannelRemainingTimeNotify(int remainingTimeInSec)
{
    
}

void MeetingWindow::onRemoteUserOnLineNotify(QString uid, int elapsed)
{
    addUser(uid);
}

void MeetingWindow::onRemoteUserOffLineNotify(QString userId, ding::rtc::RtcEngineUserOfflineReason reason)
{
    removeUser(userId);
}

void MeetingWindow::onBye(ding::rtc::RtcEngineOnByeType code)
{
    
}

void MeetingWindow::onConnectionStatusChanged(ding::rtc::RtcEngineConnectionStatus status, ding::rtc::RtcEngineConnectionStatusChangeReason reason)
{
    
}

void MeetingWindow::onNetworkQualityChanged(QString uid, ding::rtc::RtcEngineNetworkQuality upQuality, ding::rtc::RtcEngineNetworkQuality downQuality)
{
    
}

void MeetingWindow::onStatisticsLog(QString log)
{
    
}

void MeetingWindow::onOccurWarning(int warn, QString msg)
{
    
}

void MeetingWindow::onOccurError(int error, QString msg)
{
    
}

void MeetingWindow::onStatisticsLevelLog(int level, QString log)
{
    
}

void MeetingWindow::onQualityReport(const ding::rtc::QualityReport &quality)
{
    
}

void MeetingWindow::onLocalAudioStats(const ding::rtc::LocalAudioStats &localAudioStats)
{
    
}

void MeetingWindow::onLocalVideoStats(const ding::rtc::LocalVideoStats &localVideoStats)
{
    
}

void MeetingWindow::onRemoteAudioStats(const ding::rtc::RemoteAudioStats &remoteAudioStats)
{
    
}

void MeetingWindow::onRemoteVideoStats(const ding::rtc::RemoteVideoStats &remoteVideoStats)
{
    
}

void MeetingWindow::onStats(const ding::rtc::RtcEngineStats &stats)
{
    
}

void MeetingWindow::onAudioPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, QString channel)
{
    
}

void MeetingWindow::onVideoPublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, QString channel)
{
    
}

void MeetingWindow::onScreenSharePublishStateChanged(ding::rtc::RtcEnginePublishState oldState, ding::rtc::RtcEnginePublishState newState, int elapseSinceLastState, QString channel)
{
    
}

void MeetingWindow::onRemoteTrackAvailableNotify(QString uid, ding::rtc::RtcEngineAudioTrack audioTrack, ding::rtc::RtcEngineVideoTrack videoTrack)
{
    if (videoTrack == ding::rtc::RtcEngineVideoTrackCamera) {

    }
}

void MeetingWindow::onUserAudioMuted(QString uid, bool isMute)
{
    CellWidget *cell = findWidgetByUserId(uid);
    if (cell) {
        cell->updateMicIcon(isMute);
    }
}

void MeetingWindow::onUserVideoMuted(QString uid, bool isMute, ding::rtc::RtcEngineVideoTrack videoTrack)
{
    if (videoTrack == ding::rtc::RtcEngineVideoTrackCamera) {
        CellWidget *cell = findWidgetByUserId(uid);
        if (cell) {
            cell->updateCameraIcon(isMute);
        }
    }
}

void MeetingWindow::onFirstRemoteVideoFrameDrawn(QString uid, ding::rtc::RtcEngineVideoTrack videoTrack, int width, int height, int elapsed)
{
    
}

void MeetingWindow::onFirstLocalVideoFrameDrawn(int width, int height, int elapsed)
{
    
}

void MeetingWindow::onFirstVideoPacketSend(ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost)
{
    
}

void MeetingWindow::onFirstVideoPacketReceived(QString uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost)
{
    
}

void MeetingWindow::onFirstVideoFrameReceived(QString uid, ding::rtc::RtcEngineVideoTrack videoTrack, int timeCost)
{
    
}

void MeetingWindow::onFirstAudioPacketSend(int timeCost)
{
    
}

void MeetingWindow::onFirstAudioPacketReceived(QString uid, int timeCost)
{
    
}

void MeetingWindow::onAudioSubscribeStateChanged(QString uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, QString channel)
{
    
}

void MeetingWindow::onVideoSubscribeStateChanged(QString uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, QString channel)
{
    
}

void MeetingWindow::onScreenShareSubscribeStateChanged(QString uid, ding::rtc::RtcEngineSubscribeState oldState, ding::rtc::RtcEngineSubscribeState newState, int elapseSinceLastState, QString channel)
{
    
}

void MeetingWindow::onSubscribeStreamTypeChanged(QString uid, ding::rtc::RtcEngineVideoStreamType oldStreamType, ding::rtc::RtcEngineVideoStreamType newStreamType, int elapseSinceLastState, QString channel)
{
    
}

void MeetingWindow::onRecordingDeviceAudioLevel(int level)
{
    
}

void MeetingWindow::onPlayoutDeviceAudioLevel(int level)
{
    
}

void MeetingWindow::onAudioVolumeIndication(const ding::rtc::AudioVolumeInfo *speakers, unsigned int speakerNumber)
{
    
}

void MeetingWindow::onAudioDeviceStateChanged(QString deviceId, ding::rtc::RtcEngineDeviceType deviceType, ding::rtc::RtcEngineDeviceState deviceState)
{
    
}

void MeetingWindow::updateUsers() {
    static int cnt = 1;
    if (cnt == 1) {
        // addUser("me");

        cnt++;

    } else {

    }

    return;

    int addOrRemove = 0;
    if (users_.size() == 1) {
        addOrRemove = 0;
    } else if (users_.size() == MAX_USER_COUNT) {
        addOrRemove = 1;
    } else {
        addOrRemove = tlog::RandomInt(0, 2);
    }
    if (addOrRemove == 0) {
        QString userId = QString::asprintf("%03d", cnt++);
        addUser(userId);
    } else {
        int indexToRemove = tlog::RandomInt(0, users_.size());
        removeUser(users_[indexToRemove]);
    }
}

void MeetingWindow::onScreenShareClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    RtcEngine *engine = RtcEngineObject::instance()->getEngine();
    if (isScreenShare_) {
        engine->StopScreenShare();
        isScreenShare_ = false;
        currentScreenShareDeviceName_.clear();
        button->setText(kStartScreenShare);
        return;
    }

    QMenu *screenShareMenu = new QMenu(this);
    QActionGroup *screenShareActionGroup = new QActionGroup(this);
    screenShareActionGroup->setExclusive(true);

    QAction *desktopGroupTitle = screenShareMenu->addAction(kScreenShareDesktop);
    desktopGroupTitle->setEnabled(false);

    RtcEngineScreenShareType currentShareType = engine->GetCurrentScreenShareSourceType();
    RtcEngineDeviceInfoList *desktopList = engine->GetScreenShareSourceInfo(RtcEngineScreenShareDesktop);
    int count = desktopList ? desktopList->GetCount() : 0;
    for (int i = 0; i < count; i++) {
        RtcEngineDeviceInfo deviceInfo = desktopList->GetDeviceInfo(i);
        QString deviceName = deviceInfo.deviceName.c_str();
        // FIXME: sdk should handle the empty device name case
        if (deviceName.isEmpty()) {
            deviceName = deviceInfo.deviceID.c_str();
        }
        QAction *action = screenShareMenu->addAction(deviceName);
        action->setCheckable(true);
        action->setData((int)RtcEngineScreenShareDesktop);
        screenShareActionGroup->addAction(action);
        if (currentShareType == RtcEngineScreenShareDesktop && deviceName == currentScreenShareDeviceName_) {
            action->setChecked(true);
        }
    }
    if (desktopList)
        desktopList->Release();

    screenShareMenu->addSeparator();

    QAction *windowGroupTitle = screenShareMenu->addAction(kScreenShareWindow);
    windowGroupTitle->setEnabled(false);

    RtcEngineDeviceInfoList *windowList = engine->GetScreenShareSourceInfo(RtcEngineScreenShareWindow);
    count = windowList ? windowList->GetCount() : 0;
    for (int i = 0; i < windowList->GetCount(); i++) {
        RtcEngineDeviceInfo deviceInfo = windowList->GetDeviceInfo(i);
        QString deviceName = deviceInfo.deviceName.c_str();
        // FIXME: sdk should handle the empty device name case
        if (deviceName.isEmpty()) {
            deviceName = deviceInfo.deviceID.c_str();
        }
        QAction *action = screenShareMenu->addAction(deviceName);
        action->setCheckable(true);
        action->setData((int)RtcEngineScreenShareWindow);
        screenShareActionGroup->addAction(action);
        if (currentShareType == RtcEngineScreenShareWindow && deviceName == currentScreenShareDeviceName_) {
            action->setChecked(true);
        }
    }
    if (windowList)
        windowList->Release();

    connect(screenShareActionGroup, &QActionGroup::triggered, this, &MeetingWindow::onScreenShareSelected);
//    button->setMenu(screenShareMenu);
//    button->showMenu();
    QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
    screenShareMenu->exec(pos);
}

static int findDeviceByName(RtcEngineDeviceInfoList *devices, const char *deviceName, RtcEngineDeviceInfo& out) {
    int count = devices ? devices->GetCount() : 0;
    for (int i = 0; i < count; i++) {
        RtcEngineDeviceInfo deviceInfo = devices->GetDeviceInfo(i);
        if (strcmp(deviceInfo.deviceName.c_str(), deviceName) == 0) {
            out.deviceName = deviceInfo.deviceName;
            out.deviceID = deviceInfo.deviceID;
            return i;
        }
    }
    return -1;
}

static int findDeviceById(RtcEngineDeviceInfoList *devices, const char *deviceId, RtcEngineDeviceInfo& out) {
    int count = devices ? devices->GetCount() : 0;
    for (int i = 0; i < count; i++) {
        RtcEngineDeviceInfo deviceInfo = devices->GetDeviceInfo(i);
        if (strcmp(deviceInfo.deviceID.c_str(), deviceId) == 0) {
            out.deviceName = deviceInfo.deviceName;
            out.deviceID = deviceInfo.deviceID;
            return i;
        }
    }
    return -1;
}

void MeetingWindow::onScreenShareSelected(QAction *action)
{
    QVariant groupData = action->data();
    RtcEngineScreenShareType type = (RtcEngineScreenShareType)groupData.toInt();
    QString name = action->text();
    RtcEngineDeviceInfoList *devices = RtcEngineObject::instance()->getEngine()->GetScreenShareSourceInfo(type);
    RtcEngineDeviceInfo deviceInfo;
    int found = findDeviceByName(devices, name.toUtf8().constData(), deviceInfo);
    if (found == -1) {
        found = findDeviceById(devices, name.toUtf8().constData(), deviceInfo);
    }
    if (found != -1) {
        if (type == RtcEngineScreenShareDesktop) {
            RtcEngineScreenShareConfig config;
            RtcEngineObject::instance()->getEngine()->StopScreenShare();
            RtcEngineObject::instance()->getEngine()->StartScreenShareByDesktopId(deviceInfo.deviceID.c_str(), config);
        } else if (type == RtcEngineScreenShareWindow) {
            RtcEngineScreenShareConfig config;
            RtcEngineObject::instance()->getEngine()->StopScreenShare();
            RtcEngineObject::instance()->getEngine()->StartScreenShareByWindowId(deviceInfo.deviceID.c_str(), config);
        }
        isScreenShare_ = true;
        screenShareButton_->setText(kStopScreenShare);
    }
}

void MeetingWindow::addUser(QString userId) {
    qDebug("addUser: %s", qPrintable(userId));
    users_.push_back(userId);
    updateLayout();
}

void MeetingWindow::removeUser(QString userId) {
    qDebug("removeUser: %s", qPrintable(userId));
    users_.removeAll(userId);
    updateLayout();
}

CellWidget* MeetingWindow::findWidgetByUserId(QString userId) {
    if (!users_grid_)
        return nullptr;
    for (int i = 0; i < users_grid_->count(); i++) {
        QLayoutItem* const item = users_grid_->itemAt(i);
        if (dynamic_cast<QWidgetItem *>(item)) {
            CellWidget *cell = (CellWidget *)item->widget();
            if (cell->getUserId() == userId) {
                return cell;
            }
        }
    }
    return nullptr;
}

void MeetingWindow::updateLayout() {
    for (int i = 0; i < users_grid_->count(); ++i) {
        QLayoutItem* const item = users_grid_->itemAt(i);
        if (dynamic_cast<QWidgetItem *>(item)) {
            CellWidget *cell = (CellWidget *)item->widget();
            if (i == 0) {
                RtcEngineObject::instance()->localVideoPreview(nullptr, true, ding::rtc::RtcEngineRenderModeFill);
            } else {
                RtcEngineObject::instance()->remoteVideoRender(cell->getUserId(), nullptr, ding::rtc::RtcEngineRenderModeFill);
            }
        }
    }

    // 清除当前布局
    QLayoutItem *item;
    while ((item = users_grid_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // 最多显示16个视频画面
    int userCount = std::min((int)users_.size(), MAX_USER_COUNT);
    int rows = 1, columns = 1;

    // 确定行和列的数量
    if (userCount == 2) {
        columns = 2;
    } else if (userCount == 3 || userCount == 4) {
        rows = 2;
        columns = 2;
    } else if (userCount == 5 || userCount == 6) {
        rows = 2;
        columns = 3;
    } else if (userCount == 7 || userCount == 8 || userCount == 9) {
        rows = 3;
        columns = 3;
    } else if (userCount >= 10 && userCount <= 16) {
        rows = 4;
        columns = 4;
    }

    // 为每个成员添加控件到布局
    for (int i = 0; i < userCount; ++i) {
        int row = i / columns;
        int col = i % columns;
        QString userId = users_.at(i);
        auto *cell = new CellWidget(userId, this);
        users_grid_->addWidget(cell, row, col);

//        if (i == 0) {
//            RtcEngineObject::instance()->localVideoPreview((void *)cell->getRenderTarget()->winId(), true, ding::rtc::RtcEngineRenderModeFill);
//        } else {
//            RtcEngineObject::instance()->remoteVideoRender(cell->getUserId(), (void *)cell->getRenderTarget()->winId(), ding::rtc::RtcEngineRenderModeFill);
//        }
    }

    // attach video to view
    for (int i = 0; i < userCount; ++i) {
        QLayoutItem* const item = users_grid_->itemAt(i);
        if (dynamic_cast<QWidgetItem *>(item)) {
            CellWidget *cell = (CellWidget *)item->widget();
            if (i == 0) {
                RtcEngineObject::instance()->localVideoPreview((void *)cell->getRenderTarget()->winId(), true, ding::rtc::RtcEngineRenderModeFill);
            } else {
                RtcEngineObject::instance()->remoteVideoRender(cell->getUserId(), (void *)cell->getRenderTarget()->winId(), ding::rtc::RtcEngineRenderModeFill);
            }
        }
    }
}
