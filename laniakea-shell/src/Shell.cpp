#include "Shell.h"

#include <QApplication>
#include <QScreen>
#include <QEvent>
#include <QWindow>

#include <kwindowsystem.h>

#include "global.h"
#include "PopUpMenu.h"

#include <iostream>

namespace la {

Shell::Shell(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    this->setGeometry(0, 0, 1, 1);

    MenuItemDelegate *about_system = new MenuItemDelegate(nullptr);
    MenuItemDelegate *shutdown = new MenuItemDelegate(nullptr);
    about_system->set_menu_item_title("About Laniakea");
    shutdown->set_menu_item_title("Shutdown");
//    this->system_menu_delegate->add_item_delegate(about_system);
//    this->system_menu_delegate->add_item_delegate(shutdown);

    // Properties
    this->m_menu_bar_menu = QJSValue::NullValue;

    // Signals
    KWindowSystem *kWindowSystem = KWindowSystem::self();
    QObject::connect(kWindowSystem, &KWindowSystem::numberOfDesktopsChanged,
                     this, [this](int) { emit this->numberOfDesktopsChanged(); });
    QObject::connect(kWindowSystem, &KWindowSystem::currentDesktopChanged,
                     this, [this](int) { emit this->currentDesktopChanged(); });
    // Belows will connected in QML.
}

void Shell::show()
{
    this->created();
    QWidget::show();
}

void Shell::openMenu(QObject *menu)
{
    qDebug("Shell::openMenu");
    QVariant menu_type = menu->property("type");
    QVariant menu_title = menu->property("title");
    QVariant menu_items = menu->property("items");

//    PopUpMenu *m = new PopUpMenu(menu);
//    m->addAction("please...");
//    m->popup(QPoint(50, 50));
//    PopUpMenu *subm = new PopUpMenu(menu);
//    m->addMenu(subm);
//    QVariant title = menu->property("title");
//    qDebug(title.toString().toLocal8Bit());
}

void Shell::openRebusMenu(QVariantMap *menu)
{
    if ((*menu)["type"] == 0) {
        // TODO
    }
}

void Shell::setMenuBarMenu(QVariantMap *menu)
{
    // TODO
}

void Shell::focusMenuItem(int64_t index)
{
    qDebug("focusMenuItem");
}


void Shell::quit()
{
    qApp->quit();
}


//==================
// Signal handlers
//==================



// Getters
PopUpMenu* Shell::systemMenuDelegate() const
{
    return nullptr; //this->system_menu_delegate;
}

//=================
// Event handlers
//=================
bool Shell::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::WinIdChange:
    {
        std::cout << "[MenuBar(" << this << ")" << "::WinIdChange]" << std::endl;
        QRect rect = this->window()->windowHandle()->screen()->geometry();
        this->setGeometry(rect.x(), rect.y(), 1, 1); // (0, 0) not strut.

        WId wid = effectiveWinId();
        KWindowSystem::setExtendedStrut(wid,
            0, 0, 0,
            0, 0, 0,
            30, rect.x(), rect.width(),
            0, 0, 0);
        KWindowSystem::setOnDesktop(wid, NET::OnAllDesktops);

        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}

//================
// Properties
//================
QVariant Shell::systemMenu()
{
    return this->system_menu;
}

void Shell::setSystemMenu(QVariant menu)
{
    this->system_menu = menu;
//    this->system_menu_delegate->setMenu(qvariant_cast<QObject*>(menu));
}

QJSValue& Shell::menuBarMenu()
{
    return this->m_menu_bar_menu;
}

void Shell::setMenuBarMenu(QJSValue& menuBarMenu)
{
    this->m_menu_bar_menu = menuBarMenu;
    emit this->menuBarMenuChanged();
}

int Shell::numberOfDesktops()
{
    return KWindowSystem::numberOfDesktops();
}

int Shell::currentDesktop()
{
    return KWindowSystem::currentDesktop();
}

} // namespace la
