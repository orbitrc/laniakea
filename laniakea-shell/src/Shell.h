#ifndef _LA_SHELL_H
#define _LA_SHELL_H

#include <QObject>
#include <QWidget>
#include <QJSValue>
#include <QVariantMap>

#include "PopUpMenu.h"

namespace la {

class Shell : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QVariant systemMenu READ systemMenu WRITE setSystemMenu)
    Q_PROPERTY(QJSValue menuBarMenu READ menuBarMenu WRITE setMenuBarMenu NOTIFY menuBarMenuChanged)
    Q_PROPERTY(int numberOfDesktops READ numberOfDesktops NOTIFY numberOfDesktopsChanged)
    Q_PROPERTY(int currentDesktop READ currentDesktop NOTIFY currentDesktopChanged)
private:
    QVariant system_menu;
    QJSValue m_menu_bar_menu;
//    PopUpMenu *system_menu_delegate;
public:
    explicit Shell(QWidget *parent = nullptr);

    Q_INVOKABLE void openMenu(QObject *menu);
    Q_INVOKABLE void focusMenuItem(int64_t index);
    Q_INVOKABLE void setMenuBarMenu(QVariantMap *menu);
    Q_INVOKABLE void quit();

    // Getters
    PopUpMenu* systemMenuDelegate() const;

    void openRebusMenu(QVariantMap *menu);

    //=================
    // Properties
    //=================
    QVariant systemMenu();
    void setSystemMenu(QVariant menu);

    QJSValue& menuBarMenu();
    void setMenuBarMenu(QJSValue& menuBarMenu);

    int numberOfDesktops();
    int currentDesktop();

signals:
    void created();

    void registerApplicationMenu(QString menuJson);
    void registerMenuBarMenu(QString menuJson);

    void applicationMenuRegisterRequested(QString menuJson);

    void applicationMenuItemTriggered(QString path);
    void menuBarMenuItemTriggered(QString path);
    void menuItemTriggered(QString path);

    //==========================
    // Property changed signals
    //==========================
    void menuBarMenuChanged();

    void numberOfDesktopsChanged();
    void currentDesktopChanged();

public slots:
    void show();

protected:
    bool event(QEvent *event) override;
};

} // namespace la
#endif // _LA_SHELL_H
