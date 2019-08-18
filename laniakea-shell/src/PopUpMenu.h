#ifndef _LA_POP_UP_MENU_H
#define _LA_POP_UP_MENU_H

#include <QMenu>

#include "BlusherWidget.h"
#include "MenuItemDelegate.h"

namespace la {

class PopUpMenuDelegate;

class PopUpMenu : public QMenu
{
    Q_OBJECT
public:
    PopUpMenuDelegate *menu_delegate;
private:
    QObject *m_menu;
    QVariantList items;
    QString m_path;
    QRect m_menu_bar_rect;
public:
    explicit PopUpMenu(PopUpMenuDelegate *menu_delegate);

    void add_item_delegate(MenuItemDelegate *item);

    QObject* menu() const;
    void setMenu(QObject *menu);

    const QRect& menu_bar_rect() const;
    void set_menu_bar_rect(const QRect& rect);

    void set_path(const QString& path);

    // Translate json items structure to <QAction>s.
    void set_items(QVariantList items);

    static const QVariantList filter_items(QVariantList items, QString path);

    QSize sizeHint() const override;
signals:
    void focusItem(int64_t idx);
    void itemTriggered(QString path);

public slots:
    void onAboutToShow();
    void onAboutToHide();

    void onFocusItem(int idx);

    void onItemsChanged();

    void onActionHovered(QAction *action);
    void onActionTriggered(QAction *action);

protected:
    virtual void paintEvent(QPaintEvent *) override;
//    virtual void enterEvent(QEvent *) override;
    virtual void hideEvent(QHideEvent *) override;
    virtual void keyPressEvent(QKeyEvent *) override;
    virtual void leaveEvent(QEvent *) override;
    virtual void mouseMoveEvent(QMouseEvent *) override;
    virtual void mousePressEvent(QMouseEvent *) override;
    virtual void mouseReleaseEvent(QMouseEvent *) override;
};

} // namespace la

#endif // _LA_POP_UP_MENU_H
