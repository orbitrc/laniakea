#include "MenuBar.h"

#include "global.h"
#include "DebugButton.h"
#include "Menu.h"

// Qt
#include <QScreen>
#include <QQuickItem>

#include <QPushButton>

// KDE Frameworks
#include <kwindowsystem.h>

namespace la {

MenuBar::MenuBar(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    setGeometry(0, 0, 200, 30);

    this->m_focusedMenuItemIndex = -1;
    this->m_focusedExtensionIndex = -1;

    // Set background.
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(24, 24, 24)); // #181818
    setPalette(pal);

    // Kill button.
    DebugButton *killButton = new DebugButton(this);
    QObject::connect(killButton, &DebugButton::clicked,
                     this, []() { la::shell->quit(); });
    killButton->setGeometry(90, 0, 25, 25);
    killButton->show();

    // Layout.
    this->menu_bar_layout = new QHBoxLayout;
    this->menu_bar_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(this->menu_bar_layout);

    this->menu_items_layout = new QHBoxLayout;
    this->menu_items_layout->setContentsMargins(0, 0, 0, 0);
    this->menu_extensions_layout = new QHBoxLayout;
    this->menu_extensions_layout->setContentsMargins(0, 0, 0, 0);

    this->menu_bar_layout->addLayout(this->menu_items_layout);
    this->menu_bar_layout->addStretch();
    this->menu_bar_layout->addLayout(this->menu_extensions_layout);

    // System menu.
    Menu *system_menu = new Menu(this);
    QObject::connect(system_menu, &Menu::itemTriggered,
                     this, &MenuBar::onSystemMenuItemTriggered);
    system_menu->setItems(la::shell->systemMenu()["items"].toList());
    MenuBarMenuItem *systemMenuItem = new MenuBarMenuItem;
    systemMenuItem->setTitle("System");
    systemMenuItem->setImage("qrc:/assets/orbit-logo-light@256x256.png");
    systemMenuItem->setMenu(system_menu);
    this->m_menuItems.push_back(systemMenuItem);
    this->menu_items_layout->addWidget(systemMenuItem);
    systemMenuItem->show();

    //======================
    // Menu bar extensions
    //======================
    MenuBarExtension *clockExtension = new MenuBarExtension;
    clockExtension->setTitle("00:00:00");
    this->m_menuExtensions.push_back(clockExtension);
    this->menu_extensions_layout->addWidget(clockExtension);
    clockExtension->show();
}


void MenuBar::registerMenuBarExtension(QString path)
{
    MenuBarExtension *ext = new MenuBarExtension;
    ext->setPath(path);
    this->m_menuExtensions.push_back(ext);
    this->menu_extensions_layout->addWidget(ext);
    ext->show();
}


//=================
// Event handlers
//=================
bool MenuBar::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::WinIdChange:
    {
//        std::cout << "[MenuBar(" << this << ")" << "::WinIdChange]" << std::endl;
        QRect rect = this->window()->windowHandle()->screen()->geometry();
        this->setGeometry(rect.x(), rect.y(), rect.width(), 30); // (0, 0) not strut.

        WId wid = effectiveWinId();
        KWindowSystem::setExtendedStrut(wid,
            0, 0, 0,
            0, 0, 0,
            30, rect.x(), rect.width() - 1,
            0, 0, 0);
        KWindowSystem::setOnDesktop(wid, NET::OnAllDesktops);

        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}


QList<MenuBarMenuItem*> MenuBar::menuItems()
{
    return this->m_menuItems;
}


void MenuBar::onTest()
{
    qDebug() << "clockClicked received";
}

void MenuBar::onSystemMenuItemTriggered(QString path)
{
    qDebug() << path;
}

//void MenuBar::onOpenMenu(QVariantMap items)
//{
//    qDebug() << "onOpenMenu";
//}

} // namespace la
