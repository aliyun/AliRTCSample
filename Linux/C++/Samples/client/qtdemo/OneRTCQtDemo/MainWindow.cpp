#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "MeetingWindow.h"
#include "SettingsWindow.h"
#include "AppSettings.h"
#include "Utils.h"
#include "RtcEngineObject.h"

static const QStringList kAppEnvs = {
    "onertcPre", "onertcOnline"
};

static const QStringList kPreAppIds = {
    "64o1r7v2", "z7fxhu8v"
};

static const QStringList kOnlineAppIds = {
    "8ygxx5x7"
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , meetingWindow_(nullptr)
{
    ui->setupUi(this);

    ui->comboBoxAppEnv->addItems(kAppEnvs);
    QString appEnv = AppSettings::instance()->getAppEnv();
    if (!appEnv.isEmpty()) {
        ui->comboBoxAppEnv->setCurrentText(appEnv);
    } else {
        ui->comboBoxAppEnv->setCurrentIndex(0);
    }

    updateAppIdByEnv(ui->comboBoxAppEnv->currentText());
    QString appId = AppSettings::instance()->getAppId();
    if (!appId.isEmpty()) {
        ui->comboBoxAppId->setCurrentText(appId);
    } else {
        ui->comboBoxAppId->setCurrentIndex(0);
    }

    QString channelName = AppSettings::instance()->getChannelName();
    if (channelName.isEmpty())
        channelName = QString("channel_") + GetRandomString(4);
    ui->editChannelName->setText(channelName);

    QString userId = AppSettings::instance()->getUserId();
    if (userId.isEmpty())
        userId = QString("user_") + GetRandomString(4);
    ui->editUserId->setText(userId);

    QString userName = AppSettings::instance()->getUserName();
    if (!userName.isEmpty())
        ui->editUserName->setText(userName);

    ui->btnSettings->setIcon(QIcon(":/images/setting.png"));

    connect(ui->comboBoxAppEnv, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
        updateAppIdByEnv(ui->comboBoxAppEnv->currentText());
    });

    initEngine();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    destroyEngine();
}

void MainWindow::on_btnJoinChannel_clicked()
{
    // read settings
    AppSettings::instance()->setAppId(ui->comboBoxAppId->currentText());
    AppSettings::instance()->setAppEnv(ui->comboBoxAppEnv->currentText());
    AppSettings::instance()->setChannelName(ui->editChannelName->text());
    AppSettings::instance()->setUserId(ui->editUserId->text());
    AppSettings::instance()->setUserName(ui->editUserName->text());

    meetingWindow_.reset(new MeetingWindow(this));
    if (meetingWindow_) {
        meetingWindow_->show();
    }

    hide();
}

void MainWindow::on_btnSettings_clicked()
{
    if (settingsWindow_ == nullptr) {
        settingsWindow_.reset(new SettingsWindow(this));
    }
    settingsWindow_->show();
}

void MainWindow::initEngine()
{
    QString appliationDirPath = qApp->applicationDirPath();
    ding::rtc::RtcEngine::SetLogDirPath(appliationDirPath.toUtf8().constData());
    QString extras;
    RtcEngineObject::instance()->getEngine(extras);
}

void MainWindow::destroyEngine()
{
    RtcEngineObject::instance()->destroyEngine();
}

void MainWindow::updateAppIdByEnv(const QString &env)
{
    if (env == "onertcPre") {
        ui->comboBoxAppId->clear();
        ui->comboBoxAppId->addItems(kPreAppIds);
        ui->comboBoxAppId->setCurrentIndex(0);
    } else if (env == "onertcOnline") {
        ui->comboBoxAppId->clear();
        ui->comboBoxAppId->addItems(kOnlineAppIds);
        ui->comboBoxAppId->setCurrentIndex(0);
    }
}
