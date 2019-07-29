#ifndef _LA_POP_UP_MENU_H
#define _LA_POP_UP_MENU_H

#include <QMenu>

#include "BlusherWidget.h"

namespace la {


class PopUpMenu : public QMenu
{
private:
    BlusherWidget delegate;
public:
    explicit PopUpMenu(QObject *menu);

    QSize sizeHint() const override;
signals:
    void triggered(QAction *action);

    void focusItem(int64_t idx);

public slots:
    void onAboutToHide();

protected:
    virtual void leaveEvent(QEvent *) override;
};

} // namespace la

#endif // _LA_POP_UP_MENU_H
