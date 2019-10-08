#include "MenuBar.h"

#include "global.h"

// Qt
#include <QScreen>
#include <QQuickItem>

// KDE Frameworks
#include <kwindowsystem.h>

namespace la {

MenuBar::MenuBar(BlusherWidget *parent)
    : BlusherWidget(la::engine, parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    setGeometry(0, 0, 200, 30);

    setResizeMode(QQuickWidget::SizeRootObjectToView);

    setSource(QUrl("qrc:/MenuBar.qml"));

    QQuickItem *obj = rootObject();
    QObject::connect(obj, SIGNAL(clockClicked()),
                     this, SLOT(onTest()));
    QObject::connect(obj, SIGNAL(openMenu(QVariant items)),
                     this, SLOT(onOpenMenu(QVariantMap items)));
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



void MenuBar::onTest()
{
    qDebug() << "clockClicked received";
}

void MenuBar::onOpenMenu(QVariantMap items)
{
    qDebug() << "onOpenMenu";
}

} // namespace la
