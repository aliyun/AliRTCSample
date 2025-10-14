#ifndef AUDIOSETTINGSWIDGET_H
#define AUDIOSETTINGSWIDGET_H

#include <QWidget>

#include "DeviceInfo.h"

namespace Ui {
class AudioSettingsWidget;
}

class AudioSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudioSettingsWidget(QWidget *parent = nullptr);
    ~AudioSettingsWidget();

    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void initUi();

    int findMicIndex(const QString &deviceName);
    int findSpeakerIndex(const QString &deviceName);
    QString findDeviceIdByName(const QList<DeviceInfo> &deviceList, const QString &deviceName);

private:
    Ui::AudioSettingsWidget *ui;

    QList<DeviceInfo> micDevices_;
    QList<DeviceInfo> speakerDevices_;
    bool testingMic_ = false;
    bool testingSpeaker_ = false;
    QString waveFilePath_;
};

#endif // AUDIOSETTINGSWIDGET_H
