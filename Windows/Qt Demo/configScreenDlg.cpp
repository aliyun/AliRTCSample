#include "configScreenDlg.h"

configScreenDlg::configScreenDlg(AliEngine* pEngine, QWidget *parent)
	: QDialog(parent)
	, mpEngine(pEngine)
{
	ui.setupUi(this);

	connect(ui.comboBox_type, SIGNAL(currentIndexChanged(int)), this, SLOT(updateScreenSource(int)));

	QRegExp regExp("^(-?\d+)(\.\d+)?$");
	QRegExpValidator *pattern = new QRegExpValidator(regExp, this);
	ui.lineEdit_x->setValidator(pattern);
	ui.lineEdit_y->setValidator(pattern);
	ui.lineEdit_width->setValidator(pattern);
	ui.lineEdit_height->setValidator(pattern);

	init();
}

configScreenDlg::~configScreenDlg()
{
	mpEngine = nullptr;
}

void configScreenDlg::updateScreenSource(int index)
{
	mScreenShareInfo.shareType = (AliEngineScreenShareType)index;
	mpCurrentSourceList = mpEngine->GetScreenShareSourceInfo(mScreenShareInfo.shareType);
	if (mpCurrentSourceList == nullptr) return;
	ui.comboBox_source->clear();

	size_t i = 0;
	for (i = 0; i < mpCurrentSourceList->GetCount(); i++) {
		AliEngineScreenSourcInfo info = mpCurrentSourceList->GetSourceInfo(i);
		ui.comboBox_source->addItem(info.sourceName.c_str());
		if (mScreenShareInfo.shareSourceId == QString(info.sourceId.c_str()).toUInt()) {
			ui.comboBox_source->setCurrentIndex(i);
		}
	}

	if (i >= mpCurrentSourceList->GetCount())
	{
		ui.comboBox_source->setCurrentIndex(0);
		AliEngineScreenSourcInfo info = mpCurrentSourceList->GetSourceInfo(0);
		mScreenShareInfo.shareSourceId = QString(info.sourceId.c_str()).toUInt();
	}
}

void configScreenDlg::on_pushButton_ok_clicked()
{
	if (nullptr != mpEngine)
	{
		int idx = ui.comboBox_source->currentIndex();
		if (idx < mpCurrentSourceList->GetCount())
		{
			AliEngineScreenSourcInfo info = mpCurrentSourceList->GetSourceInfo(idx);
			mScreenShareInfo.shareSourceId = QString(info.sourceId.c_str()).toUInt();
		}

		mScreenShareInfo.shareConfig.shareRegion.originX = ui.lineEdit_x->text().toFloat();
		mScreenShareInfo.shareConfig.shareRegion.originY = ui.lineEdit_y->text().toFloat();
		mScreenShareInfo.shareConfig.shareRegion.width = ui.lineEdit_width->text().toFloat();
		mScreenShareInfo.shareConfig.shareRegion.height = ui.lineEdit_height->text().toFloat();

		if (AliEngineScreenShareDesktop == mScreenShareInfo.shareType)
		{
			mpEngine->StartScreenShareByDesktopId(mScreenShareInfo.shareSourceId, mScreenShareInfo.shareConfig);
		}
		else
		{
			mpEngine->StartScreenShareByWindowId(mScreenShareInfo.shareSourceId, mScreenShareInfo.shareConfig);
		}

		accept();
	}
}

void configScreenDlg::on_checkBox_region_clicked()
{
	mScreenShareInfo.shareConfig.isShareByRegion = ui.checkBox_region->isChecked();
	ui.lineEdit_x->setEnabled(mScreenShareInfo.shareConfig.isShareByRegion);
	ui.lineEdit_y->setEnabled(mScreenShareInfo.shareConfig.isShareByRegion);
	ui.lineEdit_width->setEnabled(mScreenShareInfo.shareConfig.isShareByRegion);
	ui.lineEdit_height->setEnabled(mScreenShareInfo.shareConfig.isShareByRegion);
}

void configScreenDlg::init()
{
	if (nullptr != mpEngine)
	{
		mScreenShareInfo.shareType = mpEngine->GetCurrentScreenShareSourceType();
		mScreenShareInfo.shareConfig = mpEngine->GetScreenShareConfig();

		updateScreenSource(mScreenShareInfo.shareType);
		ui.checkBox_region->setChecked(mScreenShareInfo.shareConfig.isShareByRegion);
		on_checkBox_region_clicked();

		ui.lineEdit_x->setText(QString::number(mScreenShareInfo.shareConfig.shareRegion.originX, 'f'));
		ui.lineEdit_y->setText(QString::number(mScreenShareInfo.shareConfig.shareRegion.originY, 'f'));
		ui.lineEdit_width->setText(QString::number(mScreenShareInfo.shareConfig.shareRegion.width, 'f'));
		ui.lineEdit_height->setText(QString::number(mScreenShareInfo.shareConfig.shareRegion.height, 'f'));
	}
}