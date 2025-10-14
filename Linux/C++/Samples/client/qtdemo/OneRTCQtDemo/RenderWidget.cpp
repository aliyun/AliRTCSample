#include "renderwidget.h"

#include <QEvent>

RenderWidget::RenderWidget(QString user_id, QWidget *parent)
    : QWidget{parent}
    , user_id_(user_id)
{
    render_target_ = new QWidget(this);
    render_target_->setAttribute(Qt::WA_OpaquePaintEvent);
    render_target_->setAttribute(Qt::WA_NativeWindow, true);
    // render_target_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void RenderWidget::resizeEvent(QResizeEvent* event) {
    qDebug("RenderWidget::resizeEvent");
    _resizeRenderTargetToFit();
    QWidget::resizeEvent(event);
}

void RenderWidget::showEvent(QShowEvent *event)
{
    qDebug("RenderWidget::showEvent");
    QWidget::showEvent(event);
    // render_target_->setUpdatesEnabled(true);
    // render_target_->repaint();
    _showDebugPanel();
    qDebug("RenderWidget::showEvent, rect width: %d, height: %d, left: %d, top: %d", rect().size().width(), rect().size().height(), rect().left(), rect().top());
    qDebug("RenderWidget::showEvent, render_target_ rect width: %d, height: %d, left: %d, top: %d", render_target_->rect().size().width(), render_target_->rect().size().height(), render_target_->rect().left(), render_target_->rect().top());
}

void RenderWidget::hideEvent(QHideEvent *event)
{
    qDebug("RenderWidget::hideEvent");
    QWidget::hideEvent(event);
    _hideDebugPanel();
}

bool RenderWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (debug_panel_) {
        if (watched == window()) {
            if (event->type() == QEvent::Move
                || event->type() == QEvent::Resize
                || event->type() == QEvent::LayoutRequest) {
                qDebug("RenderWidget::eventFilter, event type: %d, rect width: %d, height: %d, left: %d, top: %d", event->type(), rect().size().width(), rect().size().height(), rect().left(), rect().top());
                debug_panel_->setFixedSize(rect().size());
                debug_panel_->move(mapToGlobal(QPoint{ rect().left(), rect().top() }));
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void RenderWidget::_updateDebugLabel() {
    if (debug_label_) {
        // std::string uid = stream_helper()->GetStream(stream_id_).uid;
        debug_label_->setText(QString("%1").arg(user_id_));
    }
}

void RenderWidget::_showDebugPanel() {
    if (debug_panel_ == nullptr) {
        debug_panel_ = new QWidget(this);
        debug_panel_layout_ = new QVBoxLayout(debug_panel_);
        debug_panel_layout_->setAlignment(Qt::AlignTop);
        // debug_panel_layout_->setMargin(0);
        debug_panel_->setLayout(debug_panel_layout_);
        debug_panel_->setAttribute(Qt::WA_TranslucentBackground);
        debug_panel_->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

    }
    if (debug_label_ == nullptr) {
        debug_label_ = new QLabel(debug_panel_);
        debug_label_->setStyleSheet("QLabel{color: white; background-color: rgba(128,128,128, 0.5); font-size:20px}");
        debug_label_->setAlignment(Qt::AlignmentFlag::AlignTop);
        debug_label_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        debug_label_->show();
        debug_panel_->layout()->addWidget(debug_label_);
    }
    window()->installEventFilter(this);
    qDebug("RenderWidget::_showDebugPanel, rect width: %d, height: %d, left: %d, top: %d", rect().size().width(), rect().size().height(), rect().left(), rect().top());
    debug_panel_->setFixedSize(rect().size());
    debug_panel_->move(mapToGlobal(QPoint{ rect().left(), rect().top() }));
    debug_panel_->show();

    _updateDebugLabel();
}

void RenderWidget::_hideDebugPanel() {
    if (debug_label_) {
        window()->removeEventFilter(this);
        debug_panel_->hide();
    }
}

void RenderWidget::_resizeRenderTargetToFit() {
    const auto& size = geometry();
    double w = (double)size.width() - 4;
    double h = (double)size.height() - 4;
    double x = 2;
    double y = 2;

    render_target_->setGeometry(
        qCeil(std::max(x, 1.0)),
        qCeil(std::max(y, 1.0)),
        qFloor(std::min(w, static_cast<double>((double)size.width() - 1))),
        qFloor(std::min(h, static_cast<double>((double)size.height() - 1))));
}
