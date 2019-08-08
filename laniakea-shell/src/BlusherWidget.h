#ifndef _LA_BLUSHER_WIDGET_H
#define _LA_BLUSHER_WIDGET_H

#include <QQuickWidget>


class BlusherWidget : public QQuickWidget
{
public:
    BlusherWidget(QQmlEngine *engine, QQuickWidget *parent = nullptr);
};

#endif // _LA_BLUSHER_WIDGET_H
