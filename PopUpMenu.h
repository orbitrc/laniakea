#ifndef _LA_POP_UP_MENU_H
#define _LA_POP_UP_MENU_H

#include <QMenu>

namespace la {


class PopUpMenu : public QMenu
{
public:
    explicit PopUpMenu(QWidget *parent = nullptr);
signals:
    void triggered(QAction *action);
};

} // namespace la

#endif // _LA_POP_UP_MENU_H
