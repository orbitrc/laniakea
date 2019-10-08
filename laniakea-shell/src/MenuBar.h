#ifndef _LA_MENU_BAR_H
#define _LA_MENU_BAR_H

#include "BlusherWidget.h"

namespace la {


class MenuBar : public BlusherWidget
{
    Q_OBJECT
public:
    MenuBar(BlusherWidget *parent = nullptr);

public slots:
    void onTest();
    void onOpenMenu(QVariantMap items);

protected:
    bool event(QEvent *event) override;
};

} // namespace la

#endif // _LA_MENU_BAR_H
