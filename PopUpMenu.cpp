#include "PopUpMenu.h"

namespace la {

PopUpMenu::PopUpMenu(QWidget *parent)
    : QMenu(parent)
{
}

void PopUpMenu::triggered(QAction *action)
{
    qDebug("triggered");
}

} // namespace la
