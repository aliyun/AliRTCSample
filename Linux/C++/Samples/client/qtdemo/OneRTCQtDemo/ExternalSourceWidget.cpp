#include "ExternalSourceWidget.h"
#include "ui_ExternalSourceWidget.h"
#include "AppSettings.h"
#include "RtcEngineObject.h"
#include "external_source_pusher.h"

ExternalSourceWidget::ExternalSourceWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ExternalSourceWidget)
{
    initUi();
}

ExternalSourceWidget::~ExternalSourceWidget()
{
    delete ui;
}

void ExternalSourceWidget::initUi()
{
    ui->setupUi(this);

    ui->enableExternalVideoSource->setChecked(AppSettings::instance()->isExternalVideoSourceEnabled());
    ui->externalVideoSourceFilePath->setText(AppSettings::instance()->getExternalVideoSourceFilePath());
    int width = 0, height = 0;
    AppSettings::instance()->getExternalVideoSourceDimension(width, height);
    ui->externalVideoSourceWidth->setText(QString::number(width));
    ui->externalVideoSourceHeight->setText(QString::number(height));

    ui->enableExternalScreenSource->setChecked(AppSettings::instance()->isExternalScreenSourceEnabled());
    ui->externalScreenSourceFilePath->setText(AppSettings::instance()->getExternalScreenSourceFilePath());
    width = 0, height = 0;
    AppSettings::instance()->getExternalScreenSourceDimension(width, height);
    ui->externalScreenSourceWidth->setText(QString::number(width));
    ui->externalScreenSourceHeight->setText(QString::number(height));

    ui->enableExternalAudioSource->setChecked(AppSettings::instance()->isExternalAudioSourceEnabled());
    ui->externalAudioSourceFilePath->setText(AppSettings::instance()->getExternalAudioSourceFilePath());
    int samplerate = 0, channels = 0;
    AppSettings::instance()->getExternalAudioSourceConfiguration(samplerate, channels);
    ui->externalAudioSourceSampleRate->setText(QString::number(samplerate));
    ui->externalAudioSourceChannels->setText(QString::number(channels));

    ui->enableExternalAudioPlayout->setChecked(AppSettings::instance()->isExternalAudioPlayoutEnabled());
    ui->externalAudioPlayoutFilePath->setText(AppSettings::instance()->getExternalAudioPlayoutFilePath());
    samplerate = 0, channels = 0;
    AppSettings::instance()->getExternalAudioPlayoutConfiguration(samplerate, channels);
    ui->externalAudioPlayoutSampleRate->setText(QString::number(samplerate));
    ui->externalAudioPlayoutChannels->setText(QString::number(channels));

    externalSourcePusher_.reset(new ExternalSourcePusher(RtcEngineObject::instance()->getEngine()));

    connect(ui->enableExternalVideoSource, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked) {
            RawVideoFileConfig videoConfig;
            videoConfig.file_path = ui->externalVideoSourceFilePath->text().toUtf8().constData();
            videoConfig.width = ui->externalVideoSourceWidth->text().toInt();
            videoConfig.height = ui->externalVideoSourceHeight->text().toInt();
            // videoConfig.fps = 20;
            // videoConfig.rotation = 0;
            externalSourcePusher_->StartPushExternalVideoSource(videoConfig);
        } else {
            externalSourcePusher_->StopPushExternalVideoSource();
        }
    });
    connect(ui->enableExternalScreenSource, &QCheckBox::toggled, this, [this](bool checked) {
         if (checked) {
             RawVideoFileConfig videoConfig;
             videoConfig.file_path = ui->externalVideoSourceFilePath->text().toUtf8().constData();
             videoConfig.width = ui->externalVideoSourceWidth->text().toInt();
             videoConfig.height = ui->externalVideoSourceHeight->text().toInt();
             // videoConfig.fps = 20;
             // videoConfig.rotation = 0;
             externalSourcePusher_->StartPushExternalScreenSource(videoConfig);
         } else {
             externalSourcePusher_->StopPushExternalScreenSource();
         }
    });
    connect(ui->enableExternalAudioSource, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked) {
            RawAudioFileConfig audioConfig;
            audioConfig.file_path = ui->externalAudioSourceFilePath->text().toUtf8().constData();
            audioConfig.sample_rate = ui->externalAudioSourceSampleRate->text().toInt();
            audioConfig.channels = ui->externalAudioSourceChannels->text().toInt();
            // audioConfig.freq = 40;
            externalSourcePusher_->StartPushExternalAudioSource(audioConfig);
        } else {
            externalSourcePusher_->StopPushExternalAudioSource();
        }
    });
    connect(ui->enableExternalAudioPlayout, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked) {
            RawAudioFileConfig audioConfig;
            audioConfig.file_path = ui->externalAudioPlayoutFilePath->text().toUtf8().constData();
            audioConfig.sample_rate = ui->externalAudioPlayoutSampleRate->text().toInt();
            audioConfig.channels = ui->externalAudioPlayoutChannels->text().toInt();
            // audioConfig.freq = 40;
            externalSourcePusher_->StartPushExternalAudioRender(audioConfig);
        } else {
            externalSourcePusher_->StopPushExternalAudioRender();
        }
    });
}
