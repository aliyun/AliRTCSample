#include "SettingsWindow.h"
// #include "ui_settingswindow.h"

#include <QTabBar>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QStylePainter>
#include <QCheckBox>
#include <QPushButton>

#include "AppSettings.h"
#include "GeneralSettingsWidget.h"
#include "VideoSettingsWidget.h"
#include "AudioSettingsWidget.h"
#include "ExternalSourceWidget.h"
#include "AVFrameObserverWidget.h"


class VerticalTabBar: public QTabBar
{
public:
    QSize tabSizeHint(int index) const
    {
        QSize s = QTabBar::tabSizeHint(index);
        s.transpose();
        return s;
    }
protected:
    void paintEvent(QPaintEvent * /*event*/)
    {
        QStylePainter painter(this);
        QStyleOptionTab opt;

        for(int i = 0;i < count();i++)
        {
            initStyleOption(&opt,i);
            painter.drawControl(QStyle::CE_TabBarTabShape, opt);
            painter.save();

            QSize s = opt.rect.size();
            s.transpose();
            QRect r(QPoint(), s);
            r.moveCenter(opt.rect.center());
            opt.rect = r;

            QPoint c = tabRect(i).center();
            painter.translate(c);
            painter.rotate(90);
            painter.translate(-c);
            painter.drawControl(QStyle::CE_TabBarTabLabel,opt);
            painter.restore();
        }
    }
};

class VerticalTabWidget : public QTabWidget
{
public:
    VerticalTabWidget(QWidget *parent = nullptr) : QTabWidget(parent)
    {
        setTabBar(new VerticalTabBar);
        setTabPosition(QTabWidget::West);
        // connect(this, &QTabWidget::currentChanged, this, &TabWidget::onTabChanged);
    }

private slots:
    void onTabChanged(int index)
    {
    }
};

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , tabWidget_(nullptr)
{
    // ui->setupUi(this);
    // setWindowModality(Qt::WindowModal);
    setModal(true);
    setWindowTitle("设置");
    // setMinimumSize(800, 600);
    setFixedSize(800, 600);

    auto *tabWidget = new VerticalTabWidget(this);
    // tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *generalSettingsWidget = new GeneralSettingsWidget();
    auto *videoSettingsWidget = new VideoSettingsWidget();
    auto *audioSettingsWidget = new AudioSettingsWidget();
    auto *externalSourceSettingsWidget = new ExternalSourceWidget();
    auto *avFrameObserverWidget = new AVFrameObserverWidget();

    tabWidget->addTab(generalSettingsWidget, "通用");
    tabWidget->addTab(videoSettingsWidget, "视频");
    tabWidget->addTab(audioSettingsWidget, "音频");
    tabWidget->addTab(externalSourceSettingsWidget, "外部输入");
    tabWidget->addTab(avFrameObserverWidget, "数据回调");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tabWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

SettingsWindow::~SettingsWindow()
{
    // delete ui;
}
