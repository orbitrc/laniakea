#include <QQuickWidget>

#ifndef _LA_MENU_BAR_H
#define _LA_MENU_BAR_H

class MenuBar : public QQuickWidget
{
    Q_OBJECT
public:
    MenuBar(QQuickWidget *parent = nullptr);
    void moveToScreen(const QScreen *screen);
protected:
    bool event(QEvent *event) override;
};

#endif // _LA_MENUBAR_H
