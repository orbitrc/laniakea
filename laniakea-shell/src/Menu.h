#ifndef _LA_MENU_H
#define _LA_MENU_H

#include <QMenu>

namespace la {

class Menu : public QMenu
{
    Q_OBJECT

private:
    QVariantMap m_items;

public:
    Menu(QWidget *parent = nullptr);

    QVariantMap items() const;
    void setItems(QVariantMap items);
};

} // namespace la

#endif
