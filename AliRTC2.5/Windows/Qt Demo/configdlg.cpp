#include "configdlg.h"
#include <QFileDialog>

ConfigDlg::ConfigDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

ConfigDlg::~ConfigDlg()
{
}

void ConfigDlg::closeEvent(QCloseEvent * event)
{
}

void ConfigDlg::configInfo(bool& bAutoPub, bool& bAudio, bool& bVideo, bool& bAutoSub, bool& bTrackQuality, bool& bH5Mode)
{
	bAutoPub = ui.checkBox_autoPub->isChecked();
	bAudio = ui.checkBox_audio->isChecked();
	bVideo = ui.checkBox_video->isChecked();
	bAutoSub = ui.checkBox_autoSub->isChecked();
	bTrackQuality = ui.radioButton_quality->isChecked();
	bH5Mode = ui.checkBox_H5Mode->isChecked();
}

void ConfigDlg::on_pushButton_ok_clicked()
{
	QDialog::accept();
}
