#include "MenuBarMenuDelegate.h"

#include <QWidgetAction>
#include <QQuickWidget>
#include <QQmlEngine>

#include <kwindowsystem.h>

#include "global.h"
#include "BlusherWidget.h"


#define MENU_BAR_MENU_STYLE_SHEET "QMenu {" \
    "  border: 0px solid black;" \
    "  background-color: transparent;" \
    "}" \
    "QMenu::item {" \
    "  border: 0px solid white;" \
    "  height: 20px;" \
    "  background-color: transparent;" \
    "}" \
    "QMenu::item:selected {" \
    "  background-color: red;" \
    "}"


MenuBarMenuDelegate::MenuBarMenuDelegate(QQuickItem *parent)
    : QQuickItem(parent)
{
    this->setAcceptedMouseButtons(Qt::LeftButton);

    // Connections
    QObject::connect(this, &MenuBarMenuDelegate::systemMenuFocusedChanged,
                     this, &MenuBarMenuDelegate::onSystemMenuFocusedChanged);
}


//=================
// Properties
//=================
bool MenuBarMenuDelegate::activated() const
{
    return this->m_activated;
}


int MenuBarMenuDelegate::focusedItemIndex() const
{
    return this->m_focused_item_index;
}

void MenuBarMenuDelegate::setFocusedItemIndex(int idx)
{
    this->m_focused_item_index = idx;
    emit this->focusedItemIndexChanged();
}


QObject* MenuBarMenuDelegate::menuBar() const
{
    return this->m_menu_bar;
}

void MenuBarMenuDelegate::setMenuBar(QObject *menuBar)
{
    this->m_menu_bar = menuBar;
}


bool MenuBarMenuDelegate::systemMenuFocused() const
{
    return this->system_menu_focused;
}

void MenuBarMenuDelegate::setSystemMenuFocused(bool val)
{
    if (this->system_menu_focused != val) {
        this->system_menu_focused = val;
        emit this->systemMenuFocusedChanged();
    }
    if (val == false) {
        return;
    }
    if (this->app_menu_focused) {
        this->app_menu_focused = false;
        emit this->appMenuFocusedChanged();
    }
    if (this->m_focused_item_index != -1) {
        this->m_focused_item_index = -1;
        emit this->focusedItemIndexChanged();
    }
}


bool MenuBarMenuDelegate::appMenuFocused() const
{
    return this->app_menu_focused;
}

void MenuBarMenuDelegate::setAppMenuFocused(bool val)
{
    if (this->app_menu_focused != val) {
        this->app_menu_focused = val;
        emit this->appMenuFocusedChanged();
    }
    if (val == false) {
        return;
    }
    if (this->system_menu_focused) {
        this->system_menu_focused = false;
        emit this->systemMenuFocusedChanged();
    }
    if (this->m_focused_item_index != -1) {
        this->m_focused_item_index = -1;
        emit this->focusedItemIndexChanged();
    }
}


//======================
// Events
//======================
void MenuBarMenuDelegate::mousePressEvent(QMouseEvent *event)
{
//    la::shell->systemMenuDelegate()->popup(QPoint(0, 30));
    this->m_activated = true;
    emit this->aboutToActivate();
//    KWindowSystem::setType(KWindowSystem::activeWindow(), NET::PopupMenu);
    QQuickItem::mousePressEvent(event);
}

//=====================
// Slots
//=====================
void MenuBarMenuDelegate::onSystemMenuFocusedChanged()
{
    if (this->system_menu_focused == true) {
    } else {
        qDebug("false");
    }
}
