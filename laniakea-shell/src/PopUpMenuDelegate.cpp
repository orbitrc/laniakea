#include "PopUpMenuDelegate.h"

// Unix
#include <libgen.h>

#include <QWidgetAction>
#include <QWidget>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlEngine>
#include <QTimer>

#include "global.h"


namespace la {

PopUpMenuDelegate::PopUpMenuDelegate(QObject *parent)
    : QObject(parent)
{
    this->m_pop_up = new PopUpMenu(this);
    this->m_pop_up->menu_delegate = this;
    this->m_pop_up->set_path(this->path());
    this->focused_item_index = -1;

    QObject::connect(this->m_pop_up, &PopUpMenu::itemTriggered,
                     this, &PopUpMenuDelegate::emitSignalItemTriggered);
    QObject::connect(this->m_pop_up, &QMenu::aboutToHide,
                     this, &PopUpMenuDelegate::emitSignalClosed);
    QObject::connect(this, &PopUpMenuDelegate::closed,
                     this, &PopUpMenuDelegate::onClosed);
}

//=====================
// Properties
//=====================

QObject* PopUpMenuDelegate::menu() const
{
    return this->m_menu;
}

void PopUpMenuDelegate::setMenu(QObject *menu)
{
    this->m_menu = menu;
    this->m_pop_up->menu_delegate = this;
    this->m_pop_up->clear();
    if (menu != nullptr) {
        this->m_pop_up->setMenu(menu);
    }

    emit this->menuChanged();
}

int PopUpMenuDelegate::focusedItemIndex() const
{
    return this->focused_item_index;
}

void PopUpMenuDelegate::setFocusedItemIndex(int idx)
{
    this->focused_item_index = idx;

    emit this->focusedItemIndexChanged();
}

QString PopUpMenuDelegate::path() const
{
    return this->m_path;
}

void PopUpMenuDelegate::setPath(QString path)
{
    this->m_path = path;

    this->m_pop_up->set_path(path);

    emit this->pathChanged();
}

QVariantList PopUpMenuDelegate::items() const
{
    return this->m_items;
}

void PopUpMenuDelegate::setItems(QVariantList items)
{
    this->m_items = items;

    this->m_pop_up->set_items(PopUpMenu::filter_items(items, this->path()));

    emit this->itemsChanged();
}


//=========================
// Public member methods
//=========================


//=========================
// QML callable functions
//=========================

void PopUpMenuDelegate::show()
{
    this->m_pop_up->popup(QPoint(0, 30));
}


//==================
// Signal handlers
//==================
void PopUpMenuDelegate::onItemFocused(int idx)
{
    qDebug("PopUpMenuDelegate::onItemFocused");
    this->setFocusedItemIndex(idx);
}

void PopUpMenuDelegate::onClosed()
{
    this->setFocusedItemIndex(-1);

    QList<QAction*> items = this->m_pop_up->actions();
    for (int i = 0; i < items.length(); ++i) {
        static_cast<MenuItemDelegate*>(items[i])->set_focused(false);
    }
}

void PopUpMenuDelegate::emitSignalItemTriggered(QString path)
{
    emit this->itemTriggered(path);
}

void PopUpMenuDelegate::emitSignalClosed()
{
    emit this->closed();
}

} // namespace la
