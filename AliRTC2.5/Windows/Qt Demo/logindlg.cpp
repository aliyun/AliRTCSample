#include "logindlg.h"
#include "AliRTCSdk\include\engine_interface.h"
#include "configdlg.h"
#include "mainwindow.h"
#include "QMessageBox"

using namespace AliRTCSdk;

LoginDlg::LoginDlg(QWidget *parent)
	: QDialog(parent)
	, mpConfgDlg(nullptr)
	, mbAudio(true)
	, mbVideo(true)
	, mbAutoPub(true)
	, mbAutoSub(true)
	, mbH5Mode(true)
	, mbTrackQuality(true)
{
	ui.setupUi(this);		

	ui.label->setHidden(true);
	ui.le_ChannelId->setHidden(true);
	ui.lb_version->setText(AliEngine::GetSDKVersion());
}

LoginDlg::~LoginDlg()
{
	if (nullptr != mpConfgDlg)
		delete mpConfgDlg;
}

void LoginDlg::configInfo(QString& sUserName, bool& bAutoPub, bool& bAudio, bool& bVideo, bool& bAutoSub, bool& bTrackQuality, bool& bH5Mode)
{
	sUserName = msUserName;
	bAutoPub = mbAutoPub;
	bAudio = mbAudio;
	bVideo = mbVideo;
	bAutoSub = mbAutoSub;
	bTrackQuality = mbTrackQuality;
	bH5Mode = mbH5Mode;
}

void LoginDlg::on_btn_Config_clicked()
{
	if (nullptr == mpConfgDlg)
		mpConfgDlg = new ConfigDlg(this);

	mpConfgDlg->exec();
	mpConfgDlg->configInfo(mbAutoPub, mbAudio, mbVideo, mbAutoSub, mbTrackQuality, mbH5Mode);
}

void LoginDlg::on_btn_Login_clicked()
{
	//if (ui.le_ChannelId->text().isEmpty())
	//{
	//	QMessageBox::warning(this, tr("警告"), tr("请输入频道号..."));
	//	return;
	//}

	if (ui.le_UserName->text().isEmpty())
	{
		QMessageBox::warning(this, tr("警告"), tr("请输入用户名..."));
		return;
	}
	msUserName = ui.le_UserName->text();

	accept();
}

