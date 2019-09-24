#ifndef _LA_POP_UP_MENU_DELEGATE_H
#define _LA_POP_UP_MENU_DELEGATE_H

#include <QMenu>

#include "PopUpMenu.h"


namespace la {


class PopUpMenuDelegate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* menu READ menu WRITE setMenu NOTIFY menuChanged)
    Q_PROPERTY(int focusedItemIndex READ focusedItemIndex WRITE setFocusedItemIndex NOTIFY focusedItemIndexChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QVariantList items READ items WRITE setItems NOTIFY itemsChanged)
    Q_PROPERTY(QRect menuBarRect READ menuBarRect WRITE setMenuBarRect NOTIFY menuBarRectChanged)
private:
    // Properties
    QObject *m_menu;
    QString m_path;
    int focused_item_index;
    QVariantList m_items;
    QRect m_menu_bar_rect;

    PopUpMenu *m_pop_up;

    PopUpMenu* construct_pop_up();
public:
    explicit PopUpMenuDelegate(QObject *parent = nullptr);

    //==============
    // Properties
    //==============
    QObject* menu() const;
    void setMenu(QObject *menu);

    int focusedItemIndex() const;
    void setFocusedItemIndex(int idx);

    QString path() const;
    void setPath(QString path);

    QVariantList items() const;
    void setItems(QVariantList items);

    QRect menuBarRect() const;
    void setMenuBarRect(QRect rect);

    //=========================
    // Public member methods
    //=========================

    Q_INVOKABLE void show(int x, int y);
    Q_INVOKABLE void open(int x, int y);
    Q_INVOKABLE void close();
signals:
    //==========================
    // Property change notifies
    //==========================
    void menuChanged();
    void focusedItemIndexChanged();
    void pathChanged();
    void itemsChanged();
    void menuBarRectChanged();


    void itemFocused(int idx);
    void itemTriggered(QString path);

    void opened();
    void closed();

public slots:
    void onItemFocused(int idx);
    void onClosed();

    void emitSignalItemTriggered(QString path);
    void emitSignalClosed();
};


} // namespace la


#endif // _LA_POP_UP_MENU_DELEGATE_H
