#ifndef _LA_MENUBARMENUDELEGATE_H
#define _LA_MENUBARMENUDELEGATE_H

#include <QQuickItem>
#include <QMenu>
#include <QWidgetAction>

#include "BlusherWidget.h"
#include "PopUpMenu.h"


class MenuBarMenuDelegate : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(int height READ height WRITE setHeight)
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int y READ y WRITE setY)
    Q_PROPERTY(bool activated READ activated)
    Q_PROPERTY(int focusedItemIndex READ focusedItemIndex WRITE setFocusedItemIndex NOTIFY focusedItemIndexChanged)
    Q_PROPERTY(bool systemMenuFocused READ systemMenuFocused WRITE setSystemMenuFocused NOTIFY systemMenuFocusedChanged)
    Q_PROPERTY(bool appMenuFocused READ appMenuFocused WRITE setAppMenuFocused NOTIFY appMenuFocusedChanged)
    Q_PROPERTY(QObject* menuBar READ menuBar WRITE setMenuBar)
private:
    QObject *m_menu_bar;
    bool m_activated;
    int m_focused_item_index;
    bool system_menu_focused;
    bool app_menu_focused;
public:
    explicit MenuBarMenuDelegate(QQuickItem *parent = nullptr);

    // Properties
    bool activated() const;

    QObject* menuBar() const;
    void setMenuBar(QObject *menuBar);

    int focusedItemIndex() const;
    void setFocusedItemIndex(int idx);

    bool systemMenuFocused() const;
    void setSystemMenuFocused(bool val);

    bool appMenuFocused() const;
    void setAppMenuFocused(bool val);

signals:
    void aboutToActivate();
    void focusedItemIndexChanged();
    void systemMenuFocusedChanged();
    void appMenuFocusedChanged();

public slots:
    void onSystemMenuFocusedChanged();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
};


#endif // _LA_MENUBARMENUDELEGATE_H
