#include "MenuBarMenuExtension.h"

#include "global.h"

#include <QPainter>
#include <QQuickWidget>
#include <QQuickItem>

namespace la {

MenuBarMenuExtension::MenuBarMenuExtension(QQuickWidget *parent)
    : BlusherWidget(la::engine, parent)
{
    this->m_title = "Title";

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setClearColor(QColor(0, 0, 0, 0));
    setSource(QUrl("qrc:/MenuBarItemContent.qml"));
}

QString MenuBarMenuExtension::path() const
{
    return this->m_path;
}

void MenuBarMenuExtension::setPath(QString path)
{
    this->m_path = path;
}

QString MenuBarMenuExtension::title() const
{
    return this->m_title;
}

void MenuBarMenuExtension::setTitle(QString title)
{
    this->m_title = title;
    this->rootObject()->setProperty("title", this->m_title);
}

QString MenuBarMenuExtension::image() const
{
    return this->m_image;
}

void MenuBarMenuExtension::setImage(QString image)
{
    this->m_image = image;
    this->rootObject()->setProperty("image", this->m_image);
}

Menu* MenuBarMenuExtension::menu()
{
    return this->m_menu;
}

void MenuBarMenuExtension::setMenu(Menu *menu)
{
    this->m_menu = menu;
}

qreal MenuBarMenuExtension::implicitWidth()
{
    return this->rootObject()->property("implicitWidth").toReal();
}


QSize MenuBarMenuExtension::sizeHint() const
{
    qreal width = const_cast<MenuBarMenuExtension*>(this)->implicitWidth();
    return QSize(width, 30);
}


void MenuBarMenuExtension::mousePressEvent(QMouseEvent *)
{
    if (this->m_menu != nullptr) {
        this->m_menu->open();
    }
}

void MenuBarMenuExtension::paintEvent(QPaintEvent *)
{
}

} // namespace la
