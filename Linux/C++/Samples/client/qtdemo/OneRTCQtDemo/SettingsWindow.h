#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

// #include <QMainWindow>

// namespace Ui {
// class SettingsWindow;
// }
#include <QDialog>

class QTabWidget;

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    // Ui::SettingsWindow *ui;
    QTabWidget * tabWidget_;
};

#endif // SETTINGSWINDOW_H
