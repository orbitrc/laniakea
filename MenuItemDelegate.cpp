#include "MenuItemDelegate.h"

#include <QQuickItem>
#include <QQmlContext>

#include "global.h"
#include "PopUpMenuDelegate.h"

namespace la {

MenuItemDelegate::MenuItemDelegate(QObject *parent)
    : QWidgetAction(parent),
      widget(engine, nullptr)
{
    this->widget.setSource(QUrl("qrc:/components/LaniakeaShell/MenuItemDelegate.qml"));
    this->widget.rootObject()->setProperty("self", QVariant::fromValue(this));
    this->widget.rootObject()->setProperty("menuDelegate", QVariant::fromValue(this->parent()));
    this->widget.setClearColor(Qt::transparent);
    this->widget.setResizeMode(QQuickWidget::SizeRootObjectToView);
    this->setDefaultWidget(&this->widget);

    QObject::connect(this, &MenuItemDelegate::itemHovered,
                     this, &MenuItemDelegate::onItemHovered);
    QObject::connect(this, &MenuItemDelegate::itemTriggered,
                     this, &MenuItemDelegate::onItemTriggered);
//    QObject::connect(this, &QAction::hovered,
//                     this, &MenuItemDelegate::testSlot);
}

MenuItemDelegate::~MenuItemDelegate()
{
}

void MenuItemDelegate::set_index(int index)
{
    this->widget.rootObject()->setProperty("index", index);
}

void MenuItemDelegate::set_menu_item(QVariant menu_item)
{
    this->widget.rootObject()->setProperty("menuItem", menu_item);
    QString title = menu_item.toMap().value("title").toString();
    this->widget.rootObject()->setProperty("menuItemTitle", title);
}

void MenuItemDelegate::set_menu_item_title(QString title)
{
    this->widget.rootObject()->setProperty("menuItemTitle", title);
}

void MenuItemDelegate::set_menu_item_separator(bool val)
{
    QAction::setSeparator(val);
    this->widget.rootObject()->setProperty("menuItemSeparator", val);
}

void MenuItemDelegate::set_focused(bool val)
{
    this->widget.rootObject()->setProperty("focused", val);
}

void MenuItemDelegate::onItemHovered()
{
    QAction::hover();
    emit QAction::hovered();
}

void MenuItemDelegate::onItemTriggered()
{
    emit QAction::triggered();
//    static_cast<PopUpMenu*>(this->parent())->menu_delegate->itemTriggered()
}

//=====================
// Debug codes
//=====================
void MenuItemDelegate::testSlot()
{
    qDebug("signal: hovered");
}


} // namespace la
