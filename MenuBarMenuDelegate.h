#ifndef _LA_MENUBARMENUDELEGATE_H
#define _LA_MENUBARMENUDELEGATE_H

#include <QQuickItem>
#include <QMenu>

#include "BlusherWidget.h"
#include "PopUpMenu.h"


class MenuBarMenuActivated : public QMenu {
private:
    BlusherWidget activated_pop_up;
    la::PopUpMenu system_menu;
public:
    MenuBarMenuActivated(QWidget *parent = nullptr);
    void add_item(const char *title);
};


class MenuBarMenuDelegate : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(int height READ height WRITE setHeight)
    Q_PROPERTY(int x READ x WRITE setX)
    Q_PROPERTY(int y READ y WRITE setY)
    Q_PROPERTY(bool activated READ activated)
    Q_PROPERTY(int focusedItemIndex READ focusedItemIndex WRITE setFocusedItemIndex NOTIFY focusedItemIndexChanged)
    Q_PROPERTY(QObject* menuBar READ menuBar WRITE setMenuBar)
//    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY hourChanged)

    MenuBarMenuActivated activated_menu;
private:
    QObject *m_menu_bar;
    int m_focused_item_index;
public:
    explicit MenuBarMenuDelegate(QQuickItem *parent = nullptr);

    // Properties
    bool activated() const;

    int focusedItemIndex() const;
    void setFocusedItemIndex(int idx);

    QObject* menuBar() const;
    void setMenuBar(QObject *menuBar);

    // Enums
    enum DefaultIndex {
        SystemMenu = -2,
        AppMenu = -1
    };
    Q_ENUM(DefaultIndex)

signals:
    void focusedItemIndexChanged();

public slots:

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
};


#endif // _LA_MENUBARMENUDELEGATE_H
