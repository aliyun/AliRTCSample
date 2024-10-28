#include "shareFile.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindlg.h"
#include "configScreenDlg.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
	, mpEngine(nullptr)
	, msUserName("")
	, mbAudio(true)
	, mbVideo(true)
	, mbAutoPub(true)
	, mbAutoSub(true)
	, mbH5Mode(true)
	, mbTrackQuality(true)
	, mICol(0)
	, mIShowNum(0)
{
    ui->setupUi(this);
	this->setWindowIcon(QIcon(":/source/LOGO.ico"));
	initUserView();	

	QDir dir;
	QString str = QCoreApplication::applicationDirPath() + "\\日志";
	dir.mkpath(str);
	QByteArray arr = str.toLocal8Bit();
	AliEngine::SetLogDirPath(arr.data());
	
	connect(this, SIGNAL(sendJoin(int)), this, SLOT(joinChannelResult(int)));
	connect(this, SIGNAL(sendUpdateView()), this, SLOT(updateView()));
	connect(ui->slider_white, SIGNAL(valueChanged(int)), this, SLOT(updateWhite(int)));
	connect(ui->slider_smooth, SIGNAL(valueChanged(int)), this, SLOT(updateSmooth(int)));
	connect(this, SIGNAL(SendAudioDeviceRecordLevel(int)), this, SLOT(onRecordLevelMsg(int)));
	connect(this, SIGNAL(SendAudioDevicePlayoutLevel(int)), this, SLOT(onPlayoutLevelMsg(int)));
	connect(this, SIGNAL(SendAudioDevicePlayoutEnd()), this, SLOT(onPlayoutEndMsg()));

	initEngine();
	loginView();	
}

MainWindow::~MainWindow()
{
	removeUserView();
    delete ui;
}

void MainWindow::loginView()
{
	LoginDlg dlg(this);
	if (dlg.exec() != QDialog::Accepted)
	{
		exit(0);
	}
	dlg.configInfo(msUserName, mbAutoPub, mbAudio, mbVideo, mbAutoSub, mbTrackQuality, mbH5Mode);

	ui->pushButton_left->setHidden(true);
	ui->pushButton_right->setHidden(true);
	mICol = 0;
	mIShowNum = 0;

	joinChannel();
}

void MainWindow::initEngine()
{	
	AliEngine::SetH5CompatibleMode(mbH5Mode);

	if (nullptr == mpEngine)
		mpEngine = AliEngine::Create("");
	mpEngine->SetEngineEventListener(this);
	mpEngine->SetLogLevel(AliEngineLogLevelInfo);
	mpEngine->PublishLocalAudioStream(mbAutoPub);
	mpEngine->PublishLocalVideoStream(mbAutoPub);
	mpEngine->PublishLocalDualStream(mbTrackQuality);
	mpEngine->SetDefaultSubscribeAllRemoteAudioStreams(mbAutoSub);
	mpEngine->SetDefaultSubscribeAllRemoteVideoStreams(mbAutoSub);
}

void MainWindow::joinChannel()
{
	AliEngineAuthInfo authinfo;//备注：需要手动输入鉴权信息 https://help.aliyun.com/document_detail/146833.html?spm=5176.11065259.1996646101.searchclickresult.230e1c95nxlEcb#title-t41-agh-m2i

	//authinfo.appId = (char*)"xxxxxxx";
	//authinfo.userId = (char*)"xxxxxxxxxxxxxxxxxxxxx";
	//authinfo.channelId = (char*)"xxxxxxxxxxxxxx";
	//authinfo.nonce = (char*)"AK-xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

	//authinfo.gslbCount = 1;
	//authinfo.gslb = new char*[authinfo.gslbCount];
	//for (int i = 0; i < authinfo.gslbCount; i++)
	//{
	//	authinfo.gslb[i] = (char*)"https://xxxxxxxxxxxxxxxxxxxxx.com";
	//}

	//authinfo.agentCount = 1;
	//authinfo.agent = new char*[authinfo.agentCount];
	//for (int i = 0; i < authinfo.agentCount; i++)
	//{
	//	authinfo.agent[i] = (char*)"xxxxxxxxxxxxxx.com";
	//}

	//authinfo.token = (char*)"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	//authinfo.timestamp = 1621476611;
	QMessageBox::warning(this, tr("提示"), tr("1.可自己搭建appserver获取地址，2. 需手动输入鉴权信息！"));
	return ;

	QByteArray arrUserName = msUserName.toUtf8();
	if (nullptr != mpEngine)
		mpEngine->JoinChannel(authinfo, arrUserName.data());
}

void MainWindow::leaveChannel()
{
	if (mpEngine->IsInCall())
	{
		if (mpEngine->IsScreenSharePublished())
			on_pushButton_Screen_clicked();
		mpEngine->LeaveChannel();
	}

	removeAllUser();
}

void MainWindow::OnJoinChannelResult(int result, const char *channel, int elapsed)
{
	emit sendJoin(result);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	if (event->type() == QEvent::Close)
	{
		if (nullptr != mpEngine)
		{
			leaveChannel();
			AliEngine::Destroy();
			mpEngine = nullptr;				
		}
	}
}

void MainWindow::joinChannelResult(int result)
{
	if (0 == result)
	{
		AliEngineVideoCanvas canvas;
		canvas.displayView = (HWND)ui->lb_view->winId();
		canvas.renderMode = AliEngineRenderMode::AliEngineRenderModeStretch;
		canvas.renderMode = AliEngineRenderMode::AliEngineRenderModeAuto;
		canvas.mirrorMode = AliEngineRenderMirrorModeAllNoMirror;

		mpEngine->SetLocalViewConfig(canvas, AliEngineVideoTrackCamera);
		mpEngine->StartPreview();
	}
	else
	{
		QString strMsg = QString(tr("加入频道失败， 错误码：%1")).arg(result);
		QMessageBox::warning(this, tr("警告"), strMsg);
		loginView();
	}
}

void MainWindow::OnLeaveChannelResult(int result, AliEngineStats stats)
{

}

void MainWindow::OnRemoteUserOnLineNotify(const char *uid, int elapsed)
{
	addUser(uid);
}

void MainWindow::OnRemoteUserOffLineNotify(const char *uid, AliEngineUserOfflineReason reason)
{
	removeUser(uid);
}

void MainWindow::OnRemoteTrackAvailableNotify(const char *uid,
	AliEngineAudioTrack audioTrack,
	AliEngineVideoTrack videoTrack)
{
	updateUser(uid, videoTrack);
}

void MainWindow::initUserView()
{
	User_view* p = new User_view;
	p->pLbUserName = ui->lb_userName01;
	p->pLbView = ui->lb_view01;
	mVecUserView.push_back(p);

	p = new User_view;
	p->pLbUserName = ui->lb_userName02;
	p->pLbView = ui->lb_view02;
	mVecUserView.push_back(p);

	p = new User_view;
	p->pLbUserName = ui->lb_userName03;
	p->pLbView = ui->lb_view03;
	mVecUserView.push_back(p);

	p = new User_view;
	p->pLbUserName = ui->lb_userName04;
	p->pLbView = ui->lb_view04;
	mVecUserView.push_back(p);

	p = new User_view;
	p->pLbUserName = ui->lb_userName05;
	p->pLbView = ui->lb_view05;
	mVecUserView.push_back(p);
}

void MainWindow::removeUserView()
{
	for (int i = 0; i < mVecUserView.size(); ++i)
	{
		delete mVecUserView.at(i);
	}
}

void MainWindow::addUser(const String& uid)
{
	if (nullptr != mpEngine && mpEngine->IsInCall())
	{
		auto iter = mMapUserInfo.find(uid.c_str());
		if (iter == mMapUserInfo.end())
		{
			Dictionary dict;
			mpEngine->GetUserInfo(uid.c_str(), dict);

			User_info* pInfo = new User_info;
			pInfo->strUserName = dict.getValue("displayName").c_str();
			pInfo->vt = AliEngineVideoTrack::AliEngineVideoTrackNo;
			mMapUserInfo[uid.c_str()] = pInfo;
		}
	}
}

void MainWindow::removeUser(const String& uid)
{
	if (mpEngine->IsInCall())
	{
		auto iter = mMapUserInfo.find(uid.c_str());
		if (iter != mMapUserInfo.end())
		{
			delete iter.value();
			mMapUserInfo.remove(uid.c_str());
			emit sendUpdateView();
		}
	}
}

void MainWindow::removeAllUser()
{
	for (auto iter = mMapUserInfo.begin(); iter != mMapUserInfo.end(); ++iter)
		delete iter.value();
	mMapUserInfo.clear();
}

void MainWindow::updateUser(const String &uid, AliEngineVideoTrack videoTrack)
{
	if (mpEngine->IsInCall())
	{
		auto iter = mMapUserInfo.find(uid.c_str());
		if (iter != mMapUserInfo.end())
		{
			if (iter.value()->vt != videoTrack)
			{
				iter.value()->vt = videoTrack;
				emit sendUpdateView();
			}
		}
	}
}

void MainWindow::updateButton()
{
	ui->pushButton_left->setHidden(mICol == 0);

	if (mIShowNum >= 5)
	{
		if ((mMapUserInfo.size() - mICol) <= 1)
			ui->pushButton_right->setHidden(true);
		else
			ui->pushButton_right->setHidden(false);
	}
	else
		ui->pushButton_right->setHidden(true);	
}

void MainWindow::updateBeauty(int iWhite, int iSmooth)
{
	AliEngineBeautyConfig config;
	config.smoothnessLevel = iSmooth / 100.0;
	ui->label_smooth->setText(QString::number(config.smoothnessLevel, 'f', 2));
	config.whiteningLevel = iWhite / 100.0;
	ui->label_white->setText(QString::number(config.whiteningLevel, 'f', 2));

	if (nullptr != mpEngine)
		mpEngine->SetBeautyEffect(ui->checkBox_beauty->isChecked(), config);
}

void MainWindow::on_pushButton_left_clicked()
{
	if (mICol > 0)
	{
		--mICol;
		emit sendUpdateView();
	}
}

void MainWindow::on_pushButton_right_clicked()
{
	if (mIShowNum >= 5 && (mMapUserInfo.size() - mICol) > 1)
	{
		++mICol;
		emit sendUpdateView();
	}
}

void MainWindow::on_checkBox_muteAudio_clicked()
{
	if (nullptr != mpEngine)
	{
		mpEngine->MuteLocalMic(ui->checkBox_muteAudio->isChecked());
	}
}

void MainWindow::on_pushButton_Screen_clicked()
{
	if (!mpEngine->IsScreenSharePublished())
	{
		configScreenDlg dlg(mpEngine, this);
		if (dlg.exec() != QDialog::Accepted)
			return;

		ui->pushButton_Screen->setText(tr("停止共享"));
	}
	else
	{
		mpEngine->StopScreenShare();
		ui->pushButton_Screen->setText(tr("开启共享"));
	}
}

void MainWindow::on_checkBox_beauty_clicked()
{	
	if (nullptr != mpEngine)
	{
		AliEngineBeautyConfig config;
		config.whiteningLevel = ui->slider_white->value() / 100.0;
		config.smoothnessLevel = ui->slider_smooth->value() / 100.0;
		bool bcheck = ui->checkBox_beauty->isChecked();

		mpEngine->SetBeautyEffect(bcheck, config);
		ui->slider_white->setEnabled(bcheck);
		ui->slider_smooth->setEnabled(bcheck);
	}
}

void MainWindow::updateView()
{
	int nCol = mICol;
	int nIndex = 0;
	QByteArray arrUid;
	User_view* pView = nullptr;
	User_info* pInfo = nullptr;

	for (auto iter = mMapUserInfo.begin(); iter != mMapUserInfo.end(); ++iter)
	{
		arrUid = iter.key().toUtf8();
		pInfo = iter.value();
		if (nCol > 0)
		{
			if (pInfo->strUserName == ui->lb_userName01->text())
			{
				AliEngineVideoCanvas canvas;
				if (pInfo->vt == AliEngineVideoTrackBoth)
				{
					mpEngine->SetRemoteViewConfig(canvas, arrUid.data(), AliEngineVideoTrackCamera);
					mpEngine->SetRemoteViewConfig(canvas, arrUid.data(), AliEngineVideoTrackScreen);
				}
				else
					mpEngine->SetRemoteViewConfig(canvas, arrUid.data(), pInfo->vt);
			}
			--nCol;
			continue;
		}
		
		if (nIndex < 5)
		{
			pView = mVecUserView.at(nIndex);			
			if (pInfo->strUserName != pView->pLbUserName->text())
			{
				AliEngineVideoCanvas canvas;
				canvas.displayView = (HWND)pView->pLbView->winId();
				canvas.renderMode = AliEngineRenderMode::AliEngineRenderModeStretch;
				canvas.renderMode = AliEngineRenderMode::AliEngineRenderModeAuto;
				canvas.mirrorMode = AliEngineRenderMirrorModeAllNoMirror;
				mpEngine->SetRemoteViewConfig(canvas, arrUid.data(), AliEngineVideoTrackCamera);

				pView->pLbUserName->setText(pInfo->strUserName);
			}

			if (pInfo->vt == AliEngineVideoTrackBoth || pInfo->vt == AliEngineVideoTrackScreen)
			{
				if (++nIndex >= 5)
					break;
				pView = mVecUserView.at(nIndex);
				if (pInfo->strUserName != pView->pLbUserName->text())
				{
					AliEngineVideoCanvas canvas;
					canvas.displayView = (HWND)pView->pLbView->winId();
					canvas.renderMode = AliEngineRenderMode::AliEngineRenderModeStretch;
					canvas.renderMode = AliEngineRenderMode::AliEngineRenderModeAuto;
					canvas.mirrorMode = AliEngineRenderMirrorModeAllNoMirror;
					mpEngine->SetRemoteViewConfig(canvas, arrUid.data(), AliEngineVideoTrackScreen);
					pView->pLbUserName->setText(pInfo->strUserName);
				}
			}			
		}

		++nIndex;
	}

	mIShowNum = nIndex;
	for(; nIndex < 5; ++nIndex)
	{
		pView = mVecUserView.at(nIndex);
		pView->pLbUserName->setText("");
		pView->pLbView->update();
	}

	updateButton();
}

void MainWindow::updateWhite(int values)
{
	updateBeauty(values, ui->slider_smooth->value());
}
void MainWindow::updateSmooth(int values)
{
	updateBeauty(ui->slider_white->value(), values);
}