#ifndef AVFRAMEOBSERVERWIDGET_H
#define AVFRAMEOBSERVERWIDGET_H

#include <QWidget>
#include <QFile>
#include <QMap>
#include "RtcEngineObject.h"

namespace Ui {
class AVFrameObserverWidget;
}

class AVFrameObserverWidget
    : public QWidget
    , public ding::rtc::RtcEngineAudioFrameObserver
    , public ding::rtc::RtcEngineVideoFrameObserver
{
    Q_OBJECT

public:
    explicit AVFrameObserverWidget(QWidget *parent = nullptr);
    ~AVFrameObserverWidget();

    // --- implements RtcEngineAudioFrameObserver ---
    void OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;
    void OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;
    void OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;
    void OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame) override;

    // --- implements RtcEngineVideoFrameObserver ---
    ding::rtc::RtcEngineVideoPixelFormat GetVideoFormatPreference() override;
    bool OnCaptureVideoFrame(ding::rtc::RtcEngineVideoFrame &frame) override;
    bool OnRemoteVideoFrame(ding::rtc::String uid, ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override;
    bool OnPreEncodeVideoFrame(ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame) override;

private:
    void initUi();
    bool openFile(QFile &file, const QString &filePath);
    void closeFile(QFile &file);
    void closeAllFiles();
    int getAudioFrameBufferSize(ding::rtc::RtcEngineAudioFrame &frame);
    int getVideoFrameBufferSize(ding::rtc::RtcEngineVideoFrame &frame);

private:
    Ui::AVFrameObserverWidget *ui;
    QFile audioPlaybackFile_;
    QFile audioCapturedFile_;
    QFile audioProcessCapturedFile_;
    QFile audioPublishFile_;
    QFile videoCaptureFile_;
    QMap<QString, FILE*> videoRemoteFiles_;
    QFile videoPreEncodeFile_;
    bool enableSaveAudio_ = false;
    bool enableSaveVideo_ = false;
    bool saveAudioPlayback_ = false;
    bool saveAudioCaptured_ = false;
    bool saveAudioProcessCaptured_ = false;
    bool saveAudioPublish_ = false;
    bool saveVideoCapture_ = false;
    bool saveVideoRemote_ = false;
    bool saveVideoPreEncode_ = false;
};

#endif // AVFRAMEOBSERVERWIDGET_H
