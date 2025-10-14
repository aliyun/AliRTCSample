#include "VideoSettingsWidget.h"
#include "ui_VideoSettingsWidget.h"
#include "RtcEngineObject.h"
#include "AppSettings.h"
#include "Utils.h"

using namespace ding::rtc;

static const QStringList kRenderModeText = {
    "自动", "拉伸平铺", "填充黑边", "裁剪"
};

static const RtcEngineRenderMode kEngineRenderMode[] = {
    RtcEngineRenderModeAuto,
    RtcEngineRenderModeStretch,
    RtcEngineRenderModeFill,
    RtcEngineRenderModeCrop
};

static const QStringList kVideoDimensionsText = {
    "120x120",
    "160x120",
    "180x180",
    "240x180",
    "320x180",
    "240x240",
    "320x240",
    "424x240",
    "360x360",
    "480x360",
    "640x360",
    "480x480",
    "640x480",
    "840x480",
    "960x540",
    "960x720",
    "1280x720",
    "1920x1080"
};

static const RtcEngineVideoDimensions kEngineVideoDimensions[] = {
    VD_120x120,
    VD_160x120,
    VD_180x180,
    VD_240x180,
    VD_320x180,
    VD_240x240,
    VD_320x240,
    VD_424x240,
    VD_360x360,
    VD_480x360,
    VD_640x360,
    VD_480x480,
    VD_640x480,
    VD_840x480,
    VD_960x540,
    VD_960x720,
    VD_1280x720,
    VD_1920x1080
};

static inline bool operator==(const RtcEngineVideoDimensions &d1, const RtcEngineVideoDimensions &d2) {
    return d1.width == d2.width && d1.height == d2.height;
}


VideoSettingsWidget::VideoSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoSettingWidget)
{
    initUi();
}

VideoSettingsWidget::~VideoSettingsWidget()
{
    delete ui;
}

void VideoSettingsWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    startCameraPreview();
}

void VideoSettingsWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    // stopCameraPreview();
}

void VideoSettingsWidget::initUi()
{
    ui->setupUi(this);

    int index = -1;
    auto cameraList = RtcEngineObject::instance()->getCameraDevices();
    for (auto camera : cameraList) {
        ui->cameraDevices->addItem(camera.deviceName);
    }
    QString cameraName = AppSettings::instance()->getSelectedCamera();
    index = findCameraIndex(cameraName);
    ui->cameraDevices->setCurrentIndex(index != -1 ? index : 0);

    ui->previewVideoMirror->setChecked(AppSettings::instance()->isLocalPreviewMirrorEnabled());
    ui->pubVideoMirror->setChecked(AppSettings::instance()->isPublishVideoMirrorEnabled());

    ui->renderMode->addItems(kRenderModeText);
    int renderMode = AppSettings::instance()->getVideoRenderMode();
    index = findRenderModeTextIndex((RtcEngineRenderMode)renderMode);
    ui->renderMode->setCurrentIndex(index != -1 ? index : 0);

    updateRenderConfig();

    ui->videoEncoderDimensions->addItems(kVideoDimensionsText);
    RtcEngineVideoDimensions dimensions;
    AppSettings::instance()->getVideoEncoderDimension(dimensions.width, dimensions.height);
    index = findVideoDimensionsTextIndex(dimensions);
    ui->videoEncoderDimensions->setCurrentIndex(index != -1 ? index : 0);

    ui->videoEncoderFps->setText(QString::number(AppSettings::instance()->getVideoEncoderFps()));

    connect(ui->cameraDevices, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        QString cameraName = ui->cameraDevices->itemText(index);
        AppSettings::instance()->setSelectedCamera(cameraName);
        RtcEngineObject::instance()->setCurrentCamera(cameraName);
    });
    connect(ui->previewVideoMirror, &QCheckBox::toggled, this, [this](bool checked) {
        updateRenderConfig();
        RtcEngineObject::instance()->getEngine()->SetLocalViewConfig(renderConfig_, RtcEngineVideoTrackCamera);
    });
    connect(ui->renderMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        AppSettings::instance()->setVideoRenderMode(kEngineRenderMode[index]);

        updateRenderConfig();
        RtcEngineObject::instance()->getEngine()->SetLocalViewConfig(renderConfig_, RtcEngineVideoTrackCamera);
    });
    connect(ui->applyConfigButton, &QPushButton::clicked, this, [this]() {
        int dimensionsIndex = ui->videoEncoderDimensions->currentIndex();
        AppSettings::instance()->setVideoEncoderDimension(kEngineVideoDimensions[dimensionsIndex].width, kEngineVideoDimensions[dimensionsIndex].height);

        bool ok = false;
        int fps = ui->videoEncoderFps->text().toInt(&ok);
        if (ok) {
            AppSettings::instance()->setVideoEncoderFps(fps);
        }
        RtcEngineVideoEncoderConfiguration encoderConfig;
        encoderConfig.dimensions.width = kEngineVideoDimensions[dimensionsIndex].width;
        encoderConfig.dimensions.height = kEngineVideoDimensions[dimensionsIndex].height;
        encoderConfig.frameRate = fps;
        RtcEngineObject::instance()->getEngine()->SetVideoEncoderConfiguration(encoderConfig);
    });
}

void VideoSettingsWidget::startCameraPreview()
{
    // canvas.displayView = window;
    // canvas.renderMode = renderMode;
    // res = engine_->SetLocalViewConfig(canvas, RtcEngineVideoTrackCamera);
    // res = engine_->StartPreview();
    RtcEngineObject::instance()->getEngine()->SetLocalViewConfig(renderConfig_, RtcEngineVideoTrackCamera);
    RtcEngineObject::instance()->getEngine()->StartPreview();
}

void VideoSettingsWidget::stopCameraPreview()
{
    RtcEngineObject::instance()->getEngine()->StopPreview();
}

void VideoSettingsWidget::updateRenderConfig()
{
    renderConfig_.displayView = (void *)ui->cameraPreview->winId();
    renderConfig_.renderMode = (RtcEngineRenderMode)AppSettings::instance()->getVideoRenderMode();
    renderConfig_.mirrorMode = ui->previewVideoMirror->isChecked() ? RtcEngineRenderMirrorModeAllMirror : RtcEngineRenderMirrorModeAllNoMirror;
}

int VideoSettingsWidget::findCameraIndex(const QString &deviceName)
{
    for (int i = 0; i < ui->cameraDevices->count(); i++) {
        if (deviceName == ui->cameraDevices->itemText(i)) {
            return i;
        }
    }
    return -1;
}

int VideoSettingsWidget::findRenderModeTextIndex(RtcEngineRenderMode renderMode)
{
    for (int i = 0; i < COUNTOF(kEngineRenderMode); i++) {
        if (renderMode == kEngineRenderMode[i])
            return i;
    }
    return -1;
}

int VideoSettingsWidget::findVideoDimensionsTextIndex(const RtcEngineVideoDimensions &dimensons)
{
    for (int i = 0; i< COUNTOF(kEngineVideoDimensions); i++) {
        if (dimensons == kEngineVideoDimensions[i]) {
            return i;
        }
    }
    return -1;
}

void VideoSettingsWidget::onCameraIndexChanged(int index)
{

}
