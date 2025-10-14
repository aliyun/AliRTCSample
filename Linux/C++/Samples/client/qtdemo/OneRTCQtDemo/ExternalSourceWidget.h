#ifndef EXTERNALSOURCEWIDGET_H
#define EXTERNALSOURCEWIDGET_H

#include <memory>
#include <QWidget>

namespace Ui {
class ExternalSourceWidget;
}

class ExternalSourcePusher;

class ExternalSourceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExternalSourceWidget(QWidget *parent = nullptr);
    ~ExternalSourceWidget();

private:
    void initUi();

private:
    Ui::ExternalSourceWidget *ui;
    std::unique_ptr<ExternalSourcePusher> externalSourcePusher_;
};

#endif // EXTERNALSOURCEWIDGET_H
