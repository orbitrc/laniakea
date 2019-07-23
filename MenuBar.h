#include <QQuickWidget>

#ifndef _LA_MENU_BAR_H
#define _LA_MENU_BAR_H

class MenuBar : public QQuickWidget
{
public:
    MenuBar(const QVariant& process);
    void moveToScreen(const QScreen *screen);
protected:
    bool event(QEvent *event) override;
};

#endif // _LA_MENUBAR_H
