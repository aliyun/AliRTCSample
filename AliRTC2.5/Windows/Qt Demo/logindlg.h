#pragma once

#include <QDialog>
#include "ui_logindlg.h"

class ConfigDlg;
class LoginDlg : public QDialog
{
	Q_OBJECT

public:
	LoginDlg(QWidget *parent = Q_NULLPTR);
	~LoginDlg();

	void configInfo(QString& sUserName,bool& bAutoPub, bool& bAudio, bool& bVideo, bool& bAutoSub, bool& bTrackQuality, bool& bH5Mode);
public slots:
	void on_btn_Config_clicked();
	void on_btn_Login_clicked();
private:
	Ui::LoginDlg ui;
	ConfigDlg* mpConfgDlg;
	QString  msUserName;
	bool mbAutoPub;
	bool mbAutoSub;
	bool mbTrackQuality;
	bool mbAudio;
	bool mbVideo;
	bool mbH5Mode;
};
