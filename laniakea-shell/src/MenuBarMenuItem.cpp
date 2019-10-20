#include "MenuBarMenuItem.h"

#include "global.h"

#include <QPainter>
#include <QQuickWidget>
#include <QQuickItem>

namespace la {

MenuBarMenuItem::MenuBarMenuItem(QQuickWidget *parent)
    : BlusherWidget(la::engine, parent)
{
    this->m_title = "Title";

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setClearColor(QColor(0, 0, 0, 0));
    setSource(QUrl("qrc:/MenuBarItemContent.qml"));
}

QString MenuBarMenuItem::path() const
{
    return this->m_path;
}

void MenuBarMenuItem::setPath(QString path)
{
    this->m_path = path;
}

QString MenuBarMenuItem::title() const
{
    return this->m_title;
}

void MenuBarMenuItem::setTitle(QString title)
{
    this->m_title = title;
    this->rootObject()->setProperty("title", this->m_title);
}

QString MenuBarMenuItem::image() const
{
    return this->m_image;
}

void MenuBarMenuItem::setImage(QString image)
{
    this->m_image = image;
    this->rootObject()->setProperty("image", this->m_image);
}

Menu* MenuBarMenuItem::menu()
{
    return this->m_menu;
}

void MenuBarMenuItem::setMenu(Menu *menu)
{
    this->m_menu = menu;
}

qreal implicitWidth()
{
    return 80;
}


QSize MenuBarMenuItem::sizeHint() const
{
    return QSize(80, 30);
}


void MenuBarMenuItem::mousePressEvent(QMouseEvent *)
{
    this->m_menu->open();
}

void MenuBarMenuItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen pen(Qt::blue, 2, Qt::SolidLine);
    painter.setPen(pen);
    painter.drawLine(0, 0, width(), height());
}

} // namespace la
