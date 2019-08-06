#ifndef _LA_MENU_ITEM_DELEGATE_H
#define _LA_MENU_ITEM_DELEGATE_H

#include <QWidgetAction>

#include "BlusherWidget.h"

namespace la {


class MenuItemDelegate : public QWidgetAction
{
    Q_OBJECT
private:
    BlusherWidget widget;
public:
    MenuItemDelegate(QObject *parent);
    ~MenuItemDelegate();

    void set_index(int index);
    void set_menu_item(QVariant menu_item);
    void set_menu_item_title(QString title);
    void set_menu_item_separator(bool val);
    void set_focused(bool val);

signals:
    void itemHovered();
    void itemTriggered();
public slots:
    void onItemHovered();
    void onItemTriggered();

    void testSlot(); // For debugging
};


} // namespace la

#endif // _LA_MENU_ITEM_DELEGATE_H
