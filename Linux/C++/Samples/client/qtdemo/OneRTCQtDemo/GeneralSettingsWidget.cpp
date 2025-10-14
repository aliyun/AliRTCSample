#include "GeneralSettingsWidget.h"
#include "ui_GeneralSettingsWidget.h"
#include "AppSettings.h"
#include "RtcEngineObject.h"

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GeneralSettingsWidget)
{
    ui->setupUi(this);

    ui->publishVideo->setChecked(AppSettings::instance()->isPublishVideoEnabled());
    ui->publishAudio->setChecked(AppSettings::instance()->isPublishAudioEnabled());
    ui->subscribeAllVideo->setChecked(AppSettings::instance()->isSubscirbeAllVideoEnabled());
    ui->subscribeAllAudio->setChecked(AppSettings::instance()->isSubscirbeAllAudioEnabled());

    connect(ui->publishVideo, &QCheckBox::toggled, [](bool checked) {
        AppSettings::instance()->enablePublishVideo(checked);
        RtcEngineObject::instance()->publishLocalVideoStream(checked);
    });
    connect(ui->publishAudio, &QCheckBox::toggled, [](bool checked) {
        AppSettings::instance()->enablePublishAudio(checked);
        RtcEngineObject::instance()->publishLocalAudioStream(checked);
    });
    connect(ui->subscribeAllVideo, &QCheckBox::toggled, [](bool checked) {
        AppSettings::instance()->enableSubscribeAllVideo(checked);
        RtcEngineObject::instance()->subscribeAllRemoteVideo(checked);
    });
    connect(ui->subscribeAllAudio, &QCheckBox::toggled, [](bool checked) {
        AppSettings::instance()->enableSubscribeAllAudio(checked);
        RtcEngineObject::instance()->subscribeAllRemoteAudio(checked);
    });
}

GeneralSettingsWidget::~GeneralSettingsWidget()
{
    delete ui;
}
