#include "PopUpMenu.h"

#include <QWidgetAction>
#include <QWidget>
#include <QQuickWidget>
#include <QQuickItem>
#include <QQmlEngine>

#define POP_UP_MENU_STYLE_SHEET "QMenu {" \
    "  border: 0px solid black;" \
    "  background-color: #11ff0000;" \
    "}" \
    "QMenu::item {" \
    "  margin: 0;" \
    "  background-color: rgba(0, 255, 0, 0.5);" \
    "}"

namespace la {

PopUpMenu::PopUpMenu(QObject *menu)
    : QMenu(nullptr)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet(POP_UP_MENU_STYLE_SHEET);

    this->delegate.setSource(QUrl("qrc:/components/LaniakeaShell/PopUpMenu.qml"));
    this->delegate.rootObject()->setProperty("menu", QVariant::fromValue(menu));

    QWidgetAction *wa = new QWidgetAction(&this->delegate);
    wa->setDefaultWidget(&this->delegate);
    this->addAction(wa);

    connect(this, &PopUpMenu::aboutToHide, this, &PopUpMenu::onAboutToHide);
}

QSize PopUpMenu::sizeHint() const
{
    return QSize(this->delegate.width(), this->delegate.height());
}


//==================
// Signals
//==================
void PopUpMenu::triggered(QAction *action)
{
    qDebug("triggered");
}

//==================
// Signal handlers
//==================
void PopUpMenu::onAboutToHide()
{
    qDebug("hide");
}

//==================
// Event handlers
//==================
void PopUpMenu::leaveEvent(QEvent *)
{
    return;
}

} // namespace la
