#ifndef _LA_MENU_BAR_H
#define _LA_MENU_BAR_H

#include <QWidget>
#include <QHBoxLayout>

#include "MenuBarMenuItem.h"
#include "MenuBarMenuExtension.h"

namespace la {


class MenuBar : public QWidget
{
    Q_OBJECT
private:
    QList<MenuBarMenuItem*> m_menuItems;
    QList<MenuBarMenuExtension*> m_menuExtensions;
    QHBoxLayout *menu_bar_layout;
    QHBoxLayout *menu_items_layout;
    QHBoxLayout *menu_extensions_layout;
    int m_focusedMenuItemIndex;
    int m_focusedExtensionIndex;

public:
    MenuBar(QWidget *parent = nullptr);

    QList<MenuBarMenuItem*> menuItems();
//    MenuBarMenuItem* systemMenuItem();

public slots:
    void onTest();
//    void onOpenMenu(QVariantMap items);
    void onSystemMenuItemTriggered(QString path);

protected:
    bool event(QEvent *event) override;
};

} // namespace la

#endif // _LA_MENU_BAR_H
