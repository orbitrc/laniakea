#include "MenuBarExtension.h"

#include "global.h"

#include <QPainter>
#include <QQuickWidget>
#include <QQuickItem>

namespace la {

MenuBarExtension::MenuBarExtension(QQuickWidget *parent)
    : BlusherWidget(la::engine, parent)
{
    this->m_title = "Title";

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setClearColor(QColor(0, 0, 0, 0));
    setSource(QUrl("qrc:/MenuBarItemContent.qml"));
}

QString MenuBarExtension::path() const
{
    return this->m_path;
}

void MenuBarExtension::setPath(QString path)
{
    this->m_path = path;
}

QString MenuBarExtension::title() const
{
    return this->m_title;
}

void MenuBarExtension::setTitle(QString title)
{
    this->m_title = title;
    this->rootObject()->setProperty("title", this->m_title);
}

QString MenuBarExtension::image() const
{
    return this->m_image;
}

void MenuBarExtension::setImage(QString image)
{
    this->m_image = image;
    this->rootObject()->setProperty("image", this->m_image);
}

Menu* MenuBarExtension::menu()
{
    return this->m_menu;
}

void MenuBarExtension::setMenu(Menu *menu)
{
    this->m_menu = menu;
}

qreal MenuBarExtension::implicitWidth()
{
    return this->rootObject()->property("implicitWidth").toReal();
}


QSize MenuBarExtension::sizeHint() const
{
    qreal width = const_cast<MenuBarExtension*>(this)->implicitWidth();
    return QSize(width, 30);
}


void MenuBarExtension::mousePressEvent(QMouseEvent *)
{
    if (this->m_menu != nullptr) {
        this->m_menu->open();
    }
}

void MenuBarExtension::paintEvent(QPaintEvent *)
{
}

} // namespace la
