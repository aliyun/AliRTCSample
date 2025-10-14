#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class RenderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderWidget(QString user_id, QWidget *parent = nullptr);

    QWidget *getRenderTarget() { return render_target_; }

    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    void _resizeRenderTargetToFit();
    void _updateDebugLabel();
    void _showDebugPanel();
    void _hideDebugPanel();

signals:

private:
    QString user_id_;
    QWidget *render_target_;
    QWidget *panel_;

    QWidget* debug_panel_ = nullptr;
    QVBoxLayout* debug_panel_layout_ = nullptr;
    QLabel* debug_label_ = nullptr;
};

#endif // RENDERWIDGET_H
