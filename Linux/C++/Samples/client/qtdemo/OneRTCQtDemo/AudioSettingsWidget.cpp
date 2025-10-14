#include "AudioSettingsWidget.h"
#include "ui_AudioSettingsWidget.h"
#include "RtcEngineObject.h"
#include "AppSettings.h"

AudioSettingsWidget::AudioSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AudioSettingsWidget)
{
    initUi();
}

AudioSettingsWidget::~AudioSettingsWidget()
{
    delete ui;
}

void AudioSettingsWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

void AudioSettingsWidget::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    if (testingMic_) {
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        adm->StopRecordingDeviceTest();
        testingMic_ = !testingMic_;
    }
    if (testingSpeaker_) {
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        adm->StopPlayoutDeviceTest();
        testingSpeaker_ = !testingSpeaker_;
    }
}

void AudioSettingsWidget::initUi()
{
    ui->setupUi(this);

    micDevices_ = RtcEngineObject::instance()->getMicrophoneDevices();
    for (auto mic : micDevices_) {
        ui->micDevices->addItem(mic.deviceName);
    }
    int index = -1;
    QString micName = AppSettings::instance()->getSelectedMic();
    index = findMicIndex(micName);
    ui->micDevices->setCurrentIndex(index != -1 ? index : 0);

    speakerDevices_ = RtcEngineObject::instance()->getSpeakerDevices();
    for (auto speaker : speakerDevices_) {
        ui->speakerDevices->addItem(speaker.deviceName);
    }
    QString speakerName = AppSettings::instance()->getSelectedSpeaker();
    index = findSpeakerIndex(speakerName);
    ui->speakerDevices->setCurrentIndex(index != -1 ? index : 0);

    connect(ui->micDevices, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        QString deviceName = ui->micDevices->itemText(index);
        AppSettings::instance()->setSelectedMic(deviceName);
        QString deviceId = findDeviceIdByName(micDevices_, deviceName);
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        adm->SetCurrentRecordingDeviceId(deviceId.toUtf8().constData());
    });
    connect(ui->speakerDevices, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
        QString deviceName = ui->speakerDevices->itemText(index);
        AppSettings::instance()->setSelectedSpeaker(deviceName);
        QString deviceId = findDeviceIdByName(speakerDevices_, deviceName);
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        adm->SetCurrentPlayoutDeviceId(deviceId.toUtf8().constData());
    });
    connect(ui->testMicButton, &QPushButton::clicked, this, [=](){
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        testingMic_ = !testingMic_;
        if (testingMic_) {
            ui->testMicButton->setText("停止测试");
            adm->StartRecordingDeviceTest();
        } else {
            ui->testMicButton->setText("开始测试");
            adm->StopRecordingDeviceTest();
        }
    });
    waveFilePath_ = "/home/yuqilin/dingtalk/alirtc/demo/android/AliRtcClientDemo/demo/src/main/assets/playout_test.wav";
    connect(ui->testSpeakerButton, &QPushButton::clicked, this, [=](){
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        testingSpeaker_ = !testingSpeaker_;
        if (testingSpeaker_) {
            ui->testSpeakerButton->setText("停止测试");
            adm->StartPlayoutDeviceTest(waveFilePath_.toUtf8().constData());
        } else {
            ui->testSpeakerButton->setText("开始测试");
            adm->StopPlayoutDeviceTest();
        }
    });

    ui->inputAudioLevel->setRange(0, 100);
    connect(RtcEngineObject::instance(), &RtcEngineObject::onRecordingDeviceAudioLevel, this, [this](int level) {
        ui->inputAudioLevel->setValue(level);
    });
    ui->inputAudioVolume->setRange(0, 100);
    connect(ui->inputAudioVolume, &QSlider::valueChanged, this, [=](int value) {
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        adm->SetRecordingDeviceVolume(value);
    });
    ui->outputAudioLevel->setRange(0, 100);
    connect(RtcEngineObject::instance(), &RtcEngineObject::onPlayoutDeviceAudioLevel, this, [this](int level) {
        ui->outputAudioLevel->setValue(level);
    });
    ui->outputAudioVolume->setRange(0, 100);
    connect(ui->outputAudioVolume, &QSlider::valueChanged, this, [=](int value) {
        auto *adm = RtcEngineObject::instance()->getEngine()->GetAudioDeviceManager();
        adm->SetPlayoutDeviceVolume(value);
    });
}

int AudioSettingsWidget::findMicIndex(const QString &deviceName)
{
    for (int i = 0; i < ui->micDevices->count(); i++) {
        if (deviceName == ui->micDevices->itemText(i)) {
            return i;
        }
    }
    return -1;
}

int AudioSettingsWidget::findSpeakerIndex(const QString &deviceName)
{
    for (int i = 0; i < ui->speakerDevices->count(); i++) {
        if (deviceName == ui->speakerDevices->itemText(i)) {
            return i;
        }
    }
    return -1;
}

QString AudioSettingsWidget::findDeviceIdByName(const QList<DeviceInfo> &deviceList, const QString &deviceName)
{
    for (auto device : deviceList) {
        if (device.deviceName == deviceName) {
            return device.deviceId;
        }
    }
    return "";
}
