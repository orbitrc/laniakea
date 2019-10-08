#include "Menu.h"

namespace la {

Menu::Menu(QWidget *parent)
    : QMenu(parent)
{
}

QVariantMap Menu::items() const
{
    return this->m_items;
}

void Menu::setItems(QVariantMap items)
{
    this->m_items = items;
}

} // namespace la
