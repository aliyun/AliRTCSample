#pragma once

#include <QDialog>
#include "ui_configScreenDlg.h"
#include "AliRTCSdk\include\engine_interface.h"
using namespace AliRTCSdk;

class configScreenDlg : public QDialog
{
	Q_OBJECT

public:
	configScreenDlg(AliEngine* pEngine, QWidget *parent = Q_NULLPTR);
	~configScreenDlg();

public slots:
	void updateScreenSource(int index);
	void on_pushButton_ok_clicked();
	void on_checkBox_region_clicked();
protected:
	void init();
private:
	Ui::configScreenDlg ui;
	AliEngine* mpEngine;
	AliEngineScreenShareInfo mScreenShareInfo;
	AliEngineScreenSourceList* mpCurrentSourceList;
};
