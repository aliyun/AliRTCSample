#ifndef CELLWIDGET_H
#define CELLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class CellWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CellWidget(QString userId, QWidget *parent = nullptr);

    QString getUserId() { return userId_; }
    QWidget *getRenderTarget() { return renderTarget_; }

    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    void resizeRenderTargetToFit();
    void updateOverlayWidget();
    void showOverlayWidget();
    void hideOverlayWidget();
    void adjustOverlayWidget();

    void initUi();
    void updateMicIcon(bool on);
    void updateCameraIcon(bool on);

private:
    bool isLocalUser();

private slots:
    void micButtonClicked();
    void cameraButtonClicked();

private:
    QString userId_;
    QLabel *userIdLabel_ = nullptr;
    QWidget *renderTarget_ = nullptr;
    QWidget *overlayWidget_ = nullptr;
    QPushButton *micButton_ = nullptr;
    QPushButton *cameraButton_ = nullptr;
    QPushButton *videoStreamType_ = nullptr;
    QPushButton *moreButton_ = nullptr;
    bool micOn_ = true;
    bool cameraOn_ = true;
};

#endif // CELLWIDGET_H
