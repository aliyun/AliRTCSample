#include "AVFrameObserverWidget.h"
#include "ui_AVFrameObserverWidget.h"
#include <QDir>

AVFrameObserverWidget::AVFrameObserverWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AVFrameObserverWidget)
{
    initUi();
}

AVFrameObserverWidget::~AVFrameObserverWidget()
{
    delete ui;
}

void AVFrameObserverWidget::OnPlaybackAudioFrame(ding::rtc::RtcEngineAudioFrame &frame)
{
    if (enableSaveAudio_ && saveAudioPlayback_) {
        if (!audioPlaybackFile_.isOpen()) {
            QString fileName = QString("audio_playback_%1_%2.pcm").arg(frame.samplesPerSec).arg(frame.channels);
            QString filePath = ui->audioFrameSavePath->text() + QDir::separator() + fileName;
            openFile(audioPlaybackFile_, filePath);
        }
        audioPlaybackFile_.write((const char *)frame.buffer, getAudioFrameBufferSize(frame));
    }
}

void AVFrameObserverWidget::OnCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame)
{
    if (enableSaveAudio_ && saveAudioCaptured_) {
        if (!audioCapturedFile_.isOpen()) {
            QString fileName = QString("audio_captured_%1_%2.pcm").arg(frame.samplesPerSec).arg(frame.channels);
            QString filePath = ui->audioFrameSavePath->text() + QDir::separator() + fileName;
            openFile(audioCapturedFile_, filePath);
        }
        audioCapturedFile_.write((const char *)frame.buffer, getAudioFrameBufferSize(frame));
    }
}

void AVFrameObserverWidget::OnProcessCapturedAudioFrame(ding::rtc::RtcEngineAudioFrame &frame)
{
    if (enableSaveAudio_ && saveAudioProcessCaptured_) {
        if (!audioProcessCapturedFile_.isOpen()) {
            QString fileName = QString("audio_process_captured_%1_%2.pcm").arg(frame.samplesPerSec).arg(frame.channels);
            QString filePath = ui->audioFrameSavePath->text() + QDir::separator() + fileName;
            openFile(audioProcessCapturedFile_, filePath);
        }
        audioProcessCapturedFile_.write((const char *)frame.buffer, getAudioFrameBufferSize(frame));
    }
}

void AVFrameObserverWidget::OnPublishAudioFrame(ding::rtc::RtcEngineAudioFrame &frame)
{
    if (enableSaveAudio_ && saveAudioPublish_) {
        if (!audioPublishFile_.isOpen()) {
            QString fileName = QString("audio_publish_%1_%2.pcm").arg(frame.samplesPerSec).arg(frame.channels);
            QString filePath = ui->audioFrameSavePath->text() + QDir::separator() + fileName;
            openFile(audioPublishFile_, filePath);
        }
        audioPublishFile_.write((const char *)frame.buffer, getAudioFrameBufferSize(frame));
    }
}

ding::rtc::RtcEngineVideoPixelFormat AVFrameObserverWidget::GetVideoFormatPreference()
{
    return ding::rtc::RtcEngineVideoI420;
}

bool AVFrameObserverWidget::OnCaptureVideoFrame(ding::rtc::RtcEngineVideoFrame &frame)
{
    if (enableSaveVideo_ && saveVideoCapture_) {
        if (!videoCaptureFile_.isOpen()) {
            QString fileName = QString("video_capture_%1_%2.yuv").arg(frame.width).arg(frame.height);
            QString filePath = ui->videoFrameSavePath->text() + QDir::separator() + fileName;
            openFile(videoCaptureFile_, filePath);
        }
        videoCaptureFile_.write((const char *)frame.data, getVideoFrameBufferSize(frame));
    }
    return true;
}

bool AVFrameObserverWidget::OnRemoteVideoFrame(ding::rtc::String uid, ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame)
{
    if (enableSaveVideo_ && saveVideoRemote_) {
        QString userId(uid.c_str());
        if (!videoRemoteFiles_.contains(userId)) {
            QString fileName = QString("video_remote_%1_%2_%3.yuv").arg(userId).arg(frame.width).arg(frame.height);
            QString filePath = ui->videoFrameSavePath->text() + QDir::separator() + fileName;
            FILE *file = fopen(filePath.toUtf8().constData(), "wb+");
            if (file) {
                videoRemoteFiles_.insert(userId, file);
            }
        } else {
            FILE *file = videoRemoteFiles_[userId];
            if (file) {
                fwrite(frame.data, 1, getVideoFrameBufferSize(frame), file);
            }
        }
    }
    return true;
}

bool AVFrameObserverWidget::OnPreEncodeVideoFrame(ding::rtc::RtcEngineVideoTrack track, ding::rtc::RtcEngineVideoFrame &frame)
{
    if (enableSaveVideo_ && saveVideoPreEncode_) {
        if (!videoPreEncodeFile_.isOpen()) {
            QString fileName = QString("video_preencode_%1_%2.yuv").arg(frame.width).arg(frame.height);
            QString filePath = ui->videoFrameSavePath->text() + QDir::separator() + fileName;
            openFile(videoPreEncodeFile_, filePath);
        }
        videoPreEncodeFile_.write((const char *)frame.data, getVideoFrameBufferSize(frame));
    }
    return true;
}

void AVFrameObserverWidget::initUi()
{
    ui->setupUi(this);

    connect(ui->enableSaveVideoFrame, &QCheckBox::toggled, this, [this](bool checked){
        enableSaveVideo_ = checked;
        RtcEngineObject::instance()->getEngine()->RegisterVideoFrameObserver(enableSaveVideo_ ? this : nullptr);
    });
    connect(ui->enableCapturedVideo, &QCheckBox::toggled, this, [this](bool checked){
        saveVideoCapture_ = checked;
        RtcEngineObject::instance()->getEngine()->EnableVideoFrameObserver(saveVideoCapture_, ding::rtc::RtcEnginePositionPostCapture);
    });
    connect(ui->enablePreEncodeVideo, &QCheckBox::toggled, this, [this](bool checked){
        saveVideoPreEncode_ = checked;
        RtcEngineObject::instance()->getEngine()->EnableVideoFrameObserver(saveVideoPreEncode_, ding::rtc::RtcEnginePositionPreEncoder);
    });
    connect(ui->enableRemoteVideo, &QCheckBox::toggled, this, [this](bool checked){
        saveVideoRemote_ = checked;
        RtcEngineObject::instance()->getEngine()->EnableVideoFrameObserver(saveVideoRemote_, ding::rtc::RtcEnginePositionPreRender);
    });

    connect(ui->enableSaveAudioFrame, &QCheckBox::toggled, this, [this](bool checked){
        enableSaveAudio_ = checked;
        RtcEngineObject::instance()->getEngine()->RegisterAudioFrameObserver(enableSaveAudio_ ? this : nullptr);
    });
    connect(ui->enablePlayoutAudio, &QCheckBox::toggled, this, [this](bool checked) {
        saveAudioPlayback_ = checked;
        RtcEngineObject::instance()->getEngine()->EnableAudioFrameObserver(saveAudioPlayback_, ding::rtc::RtcEngineAudioPositionPlayback);
    });
    connect(ui->enableCapturedAudio, &QCheckBox::toggled, this, [this](bool checked) {
        saveAudioCaptured_ = checked;
        RtcEngineObject::instance()->getEngine()->EnableAudioFrameObserver(saveAudioCaptured_, ding::rtc::RtcEngineAudioPositionCaptured);
    });
    connect(ui->enableProcessedAudio, &QCheckBox::toggled, this, [this](bool checked) {
        saveAudioProcessCaptured_ = checked;
        RtcEngineObject::instance()->getEngine()->EnableAudioFrameObserver(saveAudioProcessCaptured_, ding::rtc::RtcEngineAudioPositionProcessCaptured);
    });
    connect(ui->enablePublishedAudio, &QCheckBox::toggled, this, [this](bool checked) {
        saveAudioPublish_ = checked;
        RtcEngineObject::instance()->getEngine()->EnableAudioFrameObserver(saveAudioPublish_, ding::rtc::RtcEngineAudioPositionPub);
    });
}

bool AVFrameObserverWidget::openFile(QFile &file, const QString &filePath)
{
    if (!file.isOpen()) {
        QFileInfo fileInfo(filePath);
        QString directoryPath = fileInfo.absolutePath();
        QDir directory;
        // Create the parent directories if they do not exist
        if (!directory.exists(directoryPath)) {
            if (!directory.mkpath(directoryPath)) {
                qWarning() << "Failed to create directory path:" << directoryPath;
                return false;
            }
        }
        file.setFileName(filePath);
        return file.open(QIODevice::ReadWrite);
    }
    return true;
}

void AVFrameObserverWidget::closeFile(QFile &file)
{
    file.close();
}

void AVFrameObserverWidget::closeAllFiles()
{
    closeFile(audioPlaybackFile_);
    closeFile(audioCapturedFile_);
    closeFile(audioProcessCapturedFile_);
    closeFile(audioPublishFile_);
    closeFile(videoCaptureFile_);
    for (auto it = videoRemoteFiles_.begin(); it != videoRemoteFiles_.end(); ++it) {
        fclose(it.value());
    }
    closeFile(videoPreEncodeFile_);
}

int AVFrameObserverWidget::getAudioFrameBufferSize(ding::rtc::RtcEngineAudioFrame &frame)
{
    return frame.samples * frame.channels * frame.bytesPerSample;
}

int AVFrameObserverWidget::getVideoFrameBufferSize(ding::rtc::RtcEngineVideoFrame &frame)
{
    return frame.width * frame.height * 3 / 2;
}
