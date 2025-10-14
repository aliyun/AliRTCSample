#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MeetingWindow;
class SettingsWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent* event) override;

private slots:
    void on_btnJoinChannel_clicked();
    void on_btnSettings_clicked();

private:
    void initEngine();
    void destroyEngine();
    void updateAppIdByEnv(const QString &env);

private:
    Ui::MainWindow *ui;
    std::unique_ptr<MeetingWindow> meetingWindow_;
    std::unique_ptr<SettingsWindow> settingsWindow_;
};
#endif // MAINWINDOW_H
