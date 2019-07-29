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

    this->setGeometry(0, 0, 100, 100);

    // Properties
    this->m_menu_bar_menu = QJSValue::NullValue;
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

    PopUpMenu *m = new PopUpMenu(menu);
    m->addAction("please...");
    m->popup(QPoint(50, 50));
    PopUpMenu *subm = new PopUpMenu(menu);
    m->addMenu(subm);
    QVariant title = menu->property("title");
    qDebug(title.toString().toLocal8Bit());
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

QJSValue& Shell::menuBarMenu()
{
    return this->m_menu_bar_menu;
}

void Shell::setMenuBarMenu(QJSValue& menuBarMenu)
{
    this->m_menu_bar_menu = menuBarMenu;
    emit this->menuBarMenuChanged();
}

} // namespace la
