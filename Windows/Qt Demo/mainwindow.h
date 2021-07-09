#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AliRTCSdk\include\engine_interface.h"
#include <QMap>
#include <QVector>
#include <QLabel>

using namespace AliRTCSdk;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct USER_INFO
{
	QString  strUserName;
	AliEngineVideoTrack vt;
}User_info;

typedef struct USER_VIEW
{
	QLabel* pLbUserName;
	QLabel* pLbView;
}User_view;

class MainWindow : public QMainWindow, AliEngineEventListener
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
	void loginView();
	void initEngine();
	void joinChannel();
	void leaveChannel();

signals:
	void sendJoin(int result);
	void sendUpdateView();
protected:
	virtual void OnJoinChannelResult(int result, const char *channel, int elapsed) override;
	virtual void OnLeaveChannelResult(int result, AliEngineStats stats) override;
	virtual void OnRemoteUserOnLineNotify(const char *uid, int elapsed) override;
	virtual void OnRemoteUserOffLineNotify(const char *uid, AliEngineUserOfflineReason reason) override;
	virtual void OnRemoteTrackAvailableNotify(const char *uid,
		AliEngineAudioTrack audioTrack,
		AliEngineVideoTrack videoTrack)  override;

	void initUserView();
	void removeUserView();
	void addUser(const String& uid);
	void removeUser(const String& uid);
	void removeAllUser();
	void updateUser(const String &uid, AliEngineVideoTrack videoTrack);
	void updateButton();
	void updateBeauty(int iWhite, int iSmooth);
protected slots:
	void closeEvent(QCloseEvent * event) override;
	void joinChannelResult(int result);
	void on_pushButton_left_clicked();
	void on_pushButton_right_clicked();
	void on_checkBox_muteAudio_clicked();
	void on_pushButton_Screen_clicked();
	void on_checkBox_beauty_clicked();
	void updateView();
	void updateWhite(int values);
	void updateSmooth(int values);
private:
    Ui::MainWindow *ui;
	AliEngine* mpEngine;
	QString msUserName;
	bool mbAutoPub;
	bool mbAutoSub;
	bool mbTrackQuality;
	bool mbAudio;
	bool mbVideo;
	bool mbH5Mode;

	int mICol;
	int mIShowNum;

	QVector<User_view*> mVecUserView;
	QMap<QString, User_info*> mMapUserInfo;
};
#endif // MAINWINDOW_H
