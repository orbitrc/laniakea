#include "MenuBar.h"
#include "global.h"
#include "Shell.h"

#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QScreen>

#include <kwindowsystem.h>

#include <iostream>

MenuBar::MenuBar(QQuickWidget *parent)
    : QQuickWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    // Set up Blusher.
//    this->engine()->rootContext()->setContextProperty("Process", process);
    this->engine()->addImportPath(BLUSHER_PATH);

    this->setResizeMode(QQuickWidget::SizeRootObjectToView);
    this->setSource(QStringLiteral("qrc:/main.qml"));
}

void MenuBar::moveToScreen(const QScreen *screen)
{
    QRect rect = screen->geometry();
    this->setGeometry(rect.x(), rect.y(), rect.width(), 30);
}

bool MenuBar::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::WinIdChange:
    {
        std::cout << "[MenuBar(" << this << ")" << "::WinIdChange]" << std::endl;
        QRect rect = this->window()->windowHandle()->screen()->geometry();
        this->setGeometry(rect.x(), rect.y(), rect.width(), 30);

        WId wid = effectiveWinId();
        KWindowSystem::setExtendedStrut(wid,
            0, 0, 0,
            0, 0, 0,
            this->height(), rect.x(), rect.width(),
            0, 0, 0);
        KWindowSystem::setOnDesktop(wid, NET::OnAllDesktops);

        break;
    }
    default:
        break;
    }

    return QQuickWidget::event(event);
}
