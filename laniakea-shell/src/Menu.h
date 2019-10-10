#ifndef _LA_MENU_H
#define _LA_MENU_H

#include <QMenu>

namespace la {

class Menu : public QMenu
{
    Q_OBJECT

private:
    QVariantList m_items;

public:
    Menu(QWidget *parent = nullptr);

    QVariantList items() const;
    void setItems(QVariantList items);

    void open();

signals:
    void itemTriggered(QString path);

private slots:
    void q_onTriggered(QAction *action);
};

} // namespace la

#endif
