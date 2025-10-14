#include "CellWidget.h"

#include <QEvent>
#include <QBoxLayout>
#include <QMenu>
#include <QActionGroup>
#include <QtMath>

#include "AppSettings.h"
#include "RtcEngineObject.h"

static const char* kVideoStreamTypeFHD = "超清";
static const char* kVideoStreamTypeHD = "高清";
static const char* kVideoStreamTypeSD = "标清";
static const char* kVideoStreamTypeLD = "流畅";

CellWidget::CellWidget(QString userId, QWidget *parent)
    : QWidget(parent)
    , userId_(userId) {
    initUi();
}

// void CellWidget::SetUserId(QString userId) {
//     userId_ = userId;
// }

void CellWidget::initUi() {
    if (renderTarget_ == nullptr) {
        renderTarget_ = new QWidget(this);
        renderTarget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        renderTarget_->setAttribute(Qt::WA_OpaquePaintEvent);
        renderTarget_->setAttribute(Qt::WA_NativeWindow, true);
        renderTarget_->setMouseTracking(true);
        setMouseTracking(true);
    }
    if (overlayWidget_ == nullptr) {
        overlayWidget_ = new QWidget(this);
        QHBoxLayout *overlayLayout = new QHBoxLayout(overlayWidget_);
        overlayLayout->setSpacing(10);
        overlayLayout->setAlignment(Qt::AlignBottom | Qt::AlignCenter);
        overlayWidget_->setLayout(overlayLayout);
        overlayWidget_->setAttribute(Qt::WA_TranslucentBackground);
        overlayWidget_->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        // overlayWidget_->setFixedHeight(30);
    }

    if (userIdLabel_ == nullptr) {
        userIdLabel_ = new QLabel(overlayWidget_);
        userIdLabel_->setStyleSheet("QLabel{color: white; background-color: rgba(128,128,128, 0.5); font-size:20px}");
        userIdLabel_->setAlignment(Qt::AlignmentFlag::AlignTop);
        userIdLabel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        userIdLabel_->show();
        overlayWidget_->layout()->addWidget(userIdLabel_);
    }

    if (micButton_ == nullptr) {
        micButton_ = new QPushButton();
        if (isLocalUser()) {
            // TODO: update by local user's mic status
            updateMicIcon(micOn_);
            connect(micButton_, &QPushButton::clicked, this, &CellWidget::micButtonClicked);
        } else {
            // TODO: update by remote user's mic status
            bool micOn = true;
            updateMicIcon(micOn);
            micButton_->setEnabled(false);
        }
        overlayWidget_->layout()->addWidget(micButton_);
    }
    if (cameraButton_ == nullptr) {
        cameraButton_ = new QPushButton();
        if (isLocalUser()) {
            // TODO: update by local user's camera status
            updateCameraIcon(cameraOn_);
            connect(cameraButton_, &QPushButton::clicked, this, &CellWidget::cameraButtonClicked);
        } else {
            // TODO: update by remote user's camera status
            bool userCameraOn = true;
            updateCameraIcon(userCameraOn);
            cameraButton_->setEnabled(false);
        }
        overlayWidget_->layout()->addWidget(cameraButton_);
    }
    if (!isLocalUser() && videoStreamType_ == nullptr) {
        videoStreamType_ = new QPushButton();
        QMenu *menu = new QMenu();
        QActionGroup *streamTypeActionGroup = new QActionGroup(this);
        QAction *actionFHD = menu->addAction(kVideoStreamTypeFHD);
        QAction *actionHD = menu->addAction(kVideoStreamTypeHD);
        QAction *actionSD = menu->addAction(kVideoStreamTypeSD);
        QAction *actionLD = menu->addAction(kVideoStreamTypeLD);

        actionFHD->setCheckable(true);
        actionHD->setCheckable(true);
        actionSD->setCheckable(true);
        actionLD->setCheckable(true);

        streamTypeActionGroup->setExclusive(true);
        streamTypeActionGroup->addAction(actionFHD);
        streamTypeActionGroup->addAction(actionHD);
        streamTypeActionGroup->addAction(actionSD);
        streamTypeActionGroup->addAction(actionLD);

        // TODO: update by current video stream type
        actionFHD->setChecked(true);
        videoStreamType_->setText(kVideoStreamTypeFHD);
        videoStreamType_->setMenu(menu);

        QObject::connect(streamTypeActionGroup, &QActionGroup::triggered, this, [=](QAction *action) {
            QString selectedStreamType = action->text();
            ding::rtc::RtcEngineVideoStreamType videoStreamType = ding::rtc::RtcEngineVideoStreamTypeNone;
            if (selectedStreamType == kVideoStreamTypeFHD) {
                videoStreamType = ding::rtc::RtcEngineVideoStreamTypeFHD;
            } else if (selectedStreamType == kVideoStreamTypeHD) {
                videoStreamType = ding::rtc::RtcEngineVideoStreamTypeHD;
            } else if (selectedStreamType == kVideoStreamTypeSD) {
                videoStreamType = ding::rtc::RtcEngineVideoStreamTypeSD;
            } else if (selectedStreamType == kVideoStreamTypeLD) {
                videoStreamType = ding::rtc::RtcEngineVideoStreamTypeLD;
            }
            videoStreamType_->setText(selectedStreamType);
            RtcEngineObject::instance()->setRemoteVideoStreamType(userId_, videoStreamType);
        });

        overlayWidget_->layout()->addWidget(videoStreamType_);
    }
    if (moreButton_ == nullptr) {
        moreButton_ = new QPushButton();
        moreButton_->setIcon(QIcon(":/images/more.png"));
        if (isLocalUser()) {
            bool pubAudio = AppSettings::instance()->isPublishAudioEnabled();
            bool pubVideo = AppSettings::instance()->isPublishVideoEnabled();

            QMenu *menu = new QMenu();
            QAction *actionPubAudio = menu->addAction("推送音频流");
            actionPubAudio->setCheckable(true);
            actionPubAudio->setChecked(pubAudio);
            QObject::connect(actionPubAudio, &QAction::toggled, this, [=](bool checked) {
                RtcEngineObject::instance()->publishLocalAudioStream(checked);
            });

            // menu->addAction("停止采集音频");
            menu->addSeparator();

            QAction *actionPubVideo = menu->addAction("推送视频流");
            actionPubVideo->setCheckable(true);
            actionPubVideo->setChecked(pubVideo);
            QObject::connect(actionPubVideo, &QAction::toggled, this, [=](bool checked) {
                RtcEngineObject::instance()->publishLocalVideoStream(checked);
            });

            moreButton_->setMenu(menu);
        } else {
            bool subAudio = AppSettings::instance()->isSubscirbeAllAudioEnabled();
            QMenu *menu = new QMenu();

            QAction *actionSubAudio = menu->addAction("订阅音频流");
            actionSubAudio->setCheckable(true);
            actionSubAudio->setChecked(subAudio);
            QObject::connect(actionSubAudio, &QAction::toggled, this, [=](bool checked) {
                RtcEngineObject::instance()->subscribeRemoteAudio(userId_, checked);
            });

            menu->addSeparator();

            bool subVideo = AppSettings::instance()->isSubscirbeAllVideoEnabled();

            QAction *actionSubVideo = menu->addAction("订阅视频流");
            actionSubVideo->setCheckable(true);
            actionSubVideo->setChecked(subVideo);
            QObject::connect(actionSubVideo, &QAction::toggled, this, [=](bool checked) {
                RtcEngineObject::instance()->subscribeRemoteVideo(userId_, ding::rtc::RtcEngineVideoTrackCamera, checked);
            });

            moreButton_->setMenu(menu);
        }
        overlayWidget_->layout()->addWidget(moreButton_);
    }
}

void CellWidget::updateMicIcon(bool on)
{
    if (on) {
        micButton_->setIcon(QIcon(":/images/mic_on.png"));
    } else {
        micButton_->setIcon(QIcon(":/images/mic_off.png"));
    }
}

void CellWidget::updateCameraIcon(bool on)
{
    if (on) {
        cameraButton_->setIcon(QIcon(":/images/camera_on.png"));
    } else {
        cameraButton_->setIcon(QIcon(":/images/camera_off.png"));
    }
}

bool CellWidget::isLocalUser()
{
    return userId_ == "me";
}

void CellWidget::micButtonClicked()
{
    micOn_ = !micOn_;
    updateMicIcon(micOn_);
    RtcEngineObject::instance()->muteLocalAudio(micOn_);
}

void CellWidget::cameraButtonClicked()
{
    cameraOn_ = !cameraOn_;
    updateCameraIcon(cameraOn_);
    RtcEngineObject::instance()->muteLocalVideo(cameraOn_);
    if (cameraOn_) {
        RtcEngineObject::instance()->getEngine()->StartPreview();
    } else {
        RtcEngineObject::instance()->getEngine()->StopPreview();
    }
}

void CellWidget::resizeEvent(QResizeEvent* event) {
    // qDebug("CellWidget::resizeEvent");
    resizeRenderTargetToFit();
    QWidget::resizeEvent(event);
}

void CellWidget::showEvent(QShowEvent *event) {
    qDebug("CellWidget::showEvent");
    QWidget::showEvent(event);
    showOverlayWidget();
    renderTarget_->setUpdatesEnabled(false);
    // renderTarget_->repaint();
    // qDebug("CellWidget::showEvent, rect width: %d, height: %d, left: %d, top: %d", rect().size().width(), rect().size().height(), rect().left(), rect().top());
    // qDebug("CellWidget::showEvent, renderTarget_ rect width: %d, height: %d, left: %d, top: %d", renderTarget_->rect().size().width(), renderTarget_->rect().size().height(), renderTarget_->rect().left(), renderTarget_->rect().top());
}


void CellWidget::hideEvent(QHideEvent *event) {
    qDebug("CellWidget::hideEvent");
    QWidget::hideEvent(event);
    hideOverlayWidget();
}

bool CellWidget::eventFilter(QObject *watched, QEvent *event) {
    if (overlayWidget_) {
        if (watched == window()) {
            // qDebug("CellWidget::eventFilter, event type: %d", event->type());
            if (event->type() == QEvent::Move
                || event->type() == QEvent::Resize
                || event->type() == QEvent::LayoutRequest) {
                // qDebug("CellWidget::eventFilter, event type: %d, rect width: %d, height: %d, left: %d, top: %d", event->type(), rect().size().width(), rect().size().height(), rect().left(), rect().top());
                // overlayWidget_->setFixedSize(rect().size());
                // overlayWidget_->move(mapToGlobal(QPoint{ rect().left(), rect().top() }));
                adjustOverlayWidget();
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void CellWidget::showOverlayWidget() {
    window()->installEventFilter(this);
    adjustOverlayWidget();
    updateOverlayWidget();
}

void CellWidget::hideOverlayWidget() {
    if (overlayWidget_) {
        window()->removeEventFilter(this);
        overlayWidget_->hide();
    }
}

void CellWidget::adjustOverlayWidget() {
    const QPoint pos = mapToGlobal(QPoint{rect().left() + 4, rect().bottom() - overlayWidget_->height() - 4});
    // qDebug("CellWidget::adjustOverlayWidget, pos: %d, %d", pos.x(), pos.y());
    overlayWidget_->move(pos);
    overlayWidget_->show();
}

void CellWidget::resizeRenderTargetToFit() {
    const auto& size = geometry();
    double w = (double)size.width() - 4;
    double h = (double)size.height() - 4;
    double x = 2;
    double y = 2;

    renderTarget_->setGeometry(
        qCeil(std::max(x, 1.0)),
        qCeil(std::max(y, 1.0)),
        qFloor(std::min(w, static_cast<double>((double)size.width() - 1))),
        qFloor(std::min(h, static_cast<double>((double)size.height() - 1))));
}

void CellWidget::updateOverlayWidget() {
    if (userIdLabel_) {
        // std::string uid = stream_helper()->GetStream(stream_id_).uid;
        userIdLabel_->setText(QString("%1").arg(userId_));
    }
}

