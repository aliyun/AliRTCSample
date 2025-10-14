#ifndef VIDEOSETTINGSWIDGET_H
#define VIDEOSETTINGSWIDGET_H

#include <QWidget>
#include "engine_types.h"

namespace Ui {
class VideoSettingWidget;
}

class VideoSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoSettingsWidget(QWidget *parent = nullptr);
    ~VideoSettingsWidget();

    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void initUi();
    void startCameraPreview();
    void stopCameraPreview();
    void updateRenderConfig();

    int findCameraIndex(const QString &deviceName);
    int findRenderModeTextIndex(ding::rtc::RtcEngineRenderMode renderMode);
    int findVideoDimensionsTextIndex(const ding::rtc::RtcEngineVideoDimensions &dimensons);

private slots:
    void onCameraIndexChanged(int index);

private:
    Ui::VideoSettingWidget *ui;
    ding::rtc::RtcEngineVideoCanvas renderConfig_;
};

#endif // VIDEOSETTINGSWIDGET_H
