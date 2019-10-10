#include "Menu.h"

namespace la {

Menu::Menu(QWidget *parent)
    : QMenu(parent)
{
    QObject::connect(this, &QMenu::triggered,
                     this, &Menu::q_onTriggered);
}

QVariantList Menu::items() const
{
    return this->m_items;
}

void Menu::setItems(QVariantList items)
{
    this->m_items = items;
}

void Menu::open()
{
    // Clear.
    clear();

    // Create menu items from structure.
    for (int i = 0; i < this->m_items.length(); ++i) {
        QVariantMap item = m_items[i].toMap();
        QAction *action = new QAction(this);
        if (item.value("separator", false).toBool()) {
            action->setSeparator(true);
        }
        action->setText(item["title"].toString());
        addAction(action);
    }
    exec();
}


void Menu::q_onTriggered(QAction *action)
{
    QList<QAction*> actions = QMenu::actions();
    for (int i = 0; i < actions.length(); ++i) {
        if (actions[i] == action) {
            emit this->itemTriggered(this->m_items[i].toMap()["path"].toString());
            break;
        }
    }
}

} // namespace la
