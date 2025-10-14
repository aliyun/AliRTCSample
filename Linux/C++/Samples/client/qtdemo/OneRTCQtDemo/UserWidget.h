#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>

class UserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserWidget(QString userId, QWidget *parent = nullptr);

    QString getUserId() { return userId_; }
    QWidget *getVideoView() { return videoView_; }

private slots:
    void onMuteMic();
    void onMuteCamera();

private:
    QString userId_;
    QWidget *videoView_;
};

#endif // USERWIDGET_H
