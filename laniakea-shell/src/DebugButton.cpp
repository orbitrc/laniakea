#include "DebugButton.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

namespace la {

DebugButton::DebugButton(QWidget *parent)
    : QWidget(parent)
{
    // Set background.
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(255, 0, 0));
    setPalette(pal);

    this->mouseEntered = false;

    QObject::connect(this, &DebugButton::entered,
                     this, [this]() { update(); });
    QObject::connect(this, &DebugButton::leaved,
                     this, [this]() { update(); });
}


void DebugButton::enterEvent(QEvent *)
{
    this->mouseEntered = true;

    emit this->entered();
}

void DebugButton::leaveEvent(QEvent *)
{
    this->mouseEntered = false;

    emit this->leaved();
}

void DebugButton::mousePressEvent(QMouseEvent *event)
{
    fprintf(stderr, "mouse pressed %dx%d\n", event->x(), event->y());
    emit this->clicked();
}

void DebugButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen(Qt::red, 2, Qt::SolidLine);
    if (this->mouseEntered) {
        pen.setColor(Qt::blue);
    }
    painter.setPen(pen);
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(width(), 0);
    path.lineTo(width(), height());
    path.lineTo(0, height());
    painter.fillPath(path, pen.brush());
//    painter.drawLine(0, 0, 10, 10);

}

} // namespace la
