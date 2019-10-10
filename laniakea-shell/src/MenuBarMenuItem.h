#ifndef _MENU_BAR_MENU_ITEM_H
#define _MENU_BAR_MENU_ITEM_H

#include <QWidget>
#include <QQuickWidget>
#include <QLabel>

#include "BlusherWidget.h"
#include "Menu.h"

namespace la {

class MenuBarMenuItem : public BlusherWidget
{
    Q_OBJECT
private:
    QString m_path;
    QString m_title;
    QString m_image;
    bool m_focused;
    QLabel *m_label;
    Menu *m_menu;

public:
    explicit MenuBarMenuItem(QQuickWidget *parent = nullptr);

    QString path() const;
    void setPath(QString path);

    QString title() const;
    void setTitle(QString title);

    Menu* menu();
    void setMenu(Menu *menu);

    qreal implicitWidth();

    QSize sizeHint() const override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void focusedChanged();

public slots:
};

} // namespace la

#endif // _MENU_BAR_MENU_ITEM_H
