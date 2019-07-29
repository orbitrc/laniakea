#include "MenuBarMenuDelegate.h"

#include <QWidgetAction>
#include <QQuickWidget>
#include <QQmlEngine>

#include "global.h"
#include "BlusherWidget.h"


#define MENU_BAR_MENU_STYLE_SHEET "QMenu {" \
    "  border: 0px solid black;" \
    "  background-color: transparent;" \
    "}" \
    "QMenu::item {" \
    "  border: 0px solid white;" \
    "  height: 0;" \
    "  background-color: transparent;" \
    "}"


MenuBarMenuDelegate::MenuBarMenuDelegate(QQuickItem *parent)
    : QQuickItem(parent)
{
    this->setAcceptedMouseButtons(Qt::LeftButton);
}


//=================
// Properties
//=================
bool MenuBarMenuDelegate::activated() const
{
    return this->activated_menu.isVisible();
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

void MenuBarMenuDelegate::mousePressEvent(QMouseEvent *event)
{
    this->activated_menu.popup(QPoint(0, 0));
    QQuickItem::mousePressEvent(event);
}





//================================
// MenuBarMenuActivated
//================================
MenuBarMenuActivated::MenuBarMenuActivated(QWidget *parent)
    : QMenu(parent),
      activated_pop_up(la::engine, nullptr),
      system_menu(nullptr)
{
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet(MENU_BAR_MENU_STYLE_SHEET);

    activated_pop_up.setClearColor(Qt::transparent);
    activated_pop_up.setAttribute(Qt::WA_TranslucentBackground);
    activated_pop_up.setSource(QUrl("qrc:/components/LaniakeaShell/MenuBarMenuActivated.qml"));
    QWidgetAction *wa = new QWidgetAction(&this->activated_pop_up);
    wa->setDefaultWidget(&this->activated_pop_up);
    this->addAction(wa);
    this->addMenu(&this->system_menu);
}

void MenuBarMenuActivated::add_item(const char *title)
{

}
