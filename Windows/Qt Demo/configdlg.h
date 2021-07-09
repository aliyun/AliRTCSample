#pragma once

#include <QDialog>
#include "ui_configdlg.h"
#include "shareFile.h"

class ConfigDlg : public QDialog
{
	Q_OBJECT

public:
	ConfigDlg(QWidget *parent = Q_NULLPTR);
	~ConfigDlg();

	void configInfo(bool& bAutoPub, bool& bAudio, bool& bVideo, bool& bAutoSub, bool& bTrackQuality, bool& bH5Mode);

public slots:
	void on_pushButton_ok_clicked();

	void closeEvent(QCloseEvent * event) override;

private:
	Ui::ConfigDlg ui;
};
