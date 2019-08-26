#include "PopUpMenu.h"

// Unix
#include <libgen.h>

#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlEngine>
#include <QTimer>
#include <QStylePainter>
#include <QStyleOption>
#include <QCommonStyle>

#include "global.h"
#include "PopUpMenuDelegate.h"

#define POP_UP_MENU_STYLE_SHEET "QMenu {" \
    "  border: 0px solid white;" \
    "  border-radius: 10px;" \
    "}"

namespace la {

PopUpMenu::PopUpMenu(PopUpMenuDelegate *menu_delegate)
    : QMenu(nullptr),
      menu_delegate(menu_delegate),
      m_menu(nullptr)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    QCommonStyle *cs = new QCommonStyle();
    this->setStyle(cs);
    this->setStyleSheet(POP_UP_MENU_STYLE_SHEET);

    this->setAttribute(Qt::WA_DeleteOnClose);

//    this->delegate.rootObject()->setProperty("menu", QVariant::fromValue(menu));

    connect(this, &PopUpMenu::aboutToShow, this, &PopUpMenu::onAboutToShow);
    connect(this, &PopUpMenu::aboutToHide, this, &PopUpMenu::onAboutToHide);

    connect(this, &PopUpMenu::focusItem, this, &PopUpMenu::onFocusItem);

    connect(this, &PopUpMenu::hovered, this, &PopUpMenu::onActionHovered);
    connect(this, &PopUpMenu::triggered, this, &PopUpMenu::onActionTriggered);
}

QSize PopUpMenu::sizeHint() const
{
    QSize original_size = QMenu::sizeHint();
    int width = original_size.width();
    int height = original_size.height();
    fprintf(stderr, "sizeHint: %dx%d\n", width, height);
    return QSize(width, height);
}

void PopUpMenu::add_item_delegate(MenuItemDelegate *item)
{
    this->addAction(item);
}


QObject* PopUpMenu::menu() const
{
    return this->m_menu;
}

void PopUpMenu::setMenu(QObject *menu)
{
    this->m_menu = menu;
    QVariant title_var = this->m_menu->property("title");
    QVariant path_var = this->m_menu->property("path");
    QQmlListReference items(this->m_menu, "items", engine);
    for (int i = 0; i < items.count(); ++i) {
        QObject *item = items.at(i);
        MenuItemDelegate *del = new MenuItemDelegate(this);
        del->set_index(i);
        if (item->property("separator").toBool() != true) {
            del->set_menu_item_title(item->property("title").toString());
        } else {
            del->set_menu_item_separator(true);
        }
        this->add_item_delegate(del);
    }
}

void PopUpMenu::set_items(QVariantList items)
{
    for (int i = 0; i < items.length(); ++i) {
        const QVariant& item = items.at(i);
        MenuItemDelegate *del = new MenuItemDelegate(this);
        del->set_menu_item(item);
        this->add_item_delegate(del);
    }
}

void PopUpMenu::set_path(const QString &path)
{
    this->m_path = path;
}

const QRect& PopUpMenu::menu_bar_rect() const
{
    return this->m_menu_bar_rect;
}

void PopUpMenu::set_menu_bar_rect(const QRect &rect)
{
    this->m_menu_bar_rect = rect;
}

//================================
// Non-member functions
//================================
const QVariantList PopUpMenu::filter_items(QVariantList items, QString path)
{
    QVariantList filtered;
    QString resolved_menu_path = path;
    char *resolved_item_path;

    // Strip trailing '/' character.
    if (resolved_menu_path != "/") {
        resolved_menu_path.chop(1);
    }

    for (int i = 0; i < items.length(); ++i) {
        QVariant item_path = items.at(i).toMap().value("path");
        // Copy item path.
        resolved_item_path = new char[item_path.toString().length()];
        strncpy(resolved_item_path, item_path.toString().toLocal8Bit(),
            item_path.toString().length());
        resolved_item_path[item_path.toString().length()] = '\0';
        // Get parent path.
        dirname(resolved_item_path);
        // Add if item is direct child of given path.
        if (resolved_menu_path == resolved_item_path) {
            filtered.push_back(items.at(i));
        }
        delete[] resolved_item_path;
    }

    return filtered;
}


//==================
// Signal handlers
//==================
void PopUpMenu::onAboutToShow()
{
}

void PopUpMenu::onAboutToHide()
{
}

void PopUpMenu::onFocusItem(int idx)
{
//    fprintf(stderr, "Item %d is focused. %p\n", idx, this->activeAction());
}

void PopUpMenu::onActionHovered(QAction *action)
{
//    fprintf(stderr, "action: %p, ", action);
//    fprintf(stderr, "activeAction: %p\n", this->activeAction());
    if (action != this->activeAction()) {
        QMenu::setActiveAction(action);
    }

    MenuItemDelegate *item_delegate = static_cast<MenuItemDelegate*>(action);
    item_delegate->set_focused(true);

    QList<QAction*> items = this->actions();
    for (int i = 0; i < items.length(); ++i) {
        if (items[i] == action) {
            emit this->focusItem(i);
        } else {
            static_cast<MenuItemDelegate*>(items[i])->set_focused(false);
        }
    }
}

void PopUpMenu::onActionTriggered(QAction *action)
{
    int index = 0;
    for (; index < this->actions().length(); ++index) {
        if (action == this->actions().at(index)) {
            break;
        }
    }
    const QVariantMap& item = this->menu_delegate->property("items").toList().at(index).toMap();
    const QString path = item.value("path").toString();
    emit this->itemTriggered(path);
    fprintf(stderr, "triggered: %s\n", path.toStdString().c_str());
//    emit la::shell->menuItemTriggered(path);

    if (!this->isHidden()) {
        this->close();
    }
}

// Connect to PopUpMenuDelegate::itemsChanged.
void PopUpMenu::onItemsChanged()
{
    QVariantList items = PopUpMenu::filter_items(
        this->menu_delegate->property("items").toList(),
        this->m_path);
}


//==================
// Event handlers
//==================
void PopUpMenu::paintEvent(QPaintEvent *event)
{
//    QStylePainter style_painter(this);
//    QStyleOption option;
//    option.initFrom(this);
//    QPalette p;
//    option.palette = p;
//    style_painter.drawPrimitive(QStyle::PE_Widget, option);
    QMenu::paintEvent(event);
}

void PopUpMenu::hideEvent(QHideEvent *event)
{
    QMenu::hideEvent(event);
}

void PopUpMenu::keyPressEvent(QKeyEvent *event)
{
    QList<QAction*> actions = this->actions();
    QAction *action = this->activeAction();
    int index = -1;
    for (int i = 0; i < actions.length(); ++i) {
        if (action == actions[i]) {
            index = i;
            break;
        }
    }

    if (event->key() == Qt::Key_Up && index == 0) {
        return;
    }
    if (event->key() == Qt::Key_Down && index == actions.length() - 1) {
        return;
    }
    QMenu::keyPressEvent(event);
}

void PopUpMenu::leaveEvent(QEvent *event)
{
    QMenu::leaveEvent(event);
}

void PopUpMenu::mouseMoveEvent(QMouseEvent *event)
{
    // All mouse move event will processed in QML.
    return;
    QMenu::mouseMoveEvent(event);
}

void PopUpMenu::mousePressEvent(QMouseEvent *event)
{
    QMenu::mousePressEvent(event);
}

void PopUpMenu::mouseReleaseEvent(QMouseEvent *event)
{
    if (!this->menu_bar_rect().contains(event->globalPos()) &&
            !this->underMouse()) {
        this->close();
    } else {
        QMenu::mouseReleaseEvent(event);
    }
}

} // namespace la
