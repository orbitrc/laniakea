#ifndef _LA_SHELL_H
#define _LA_SHELL_H

#include <QObject>
#include <QWidget>
#include <QJSValue>
#include <QVariantMap>

#include <libudev.h>

#include "PopUpMenu.h"
#include "MenuBar.h"
#include "ConfFile.h"

namespace la {

class Shell : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QVariant systemMenu READ systemMenu WRITE setSystemMenu)
    Q_PROPERTY(QJSValue menuBarMenu READ menuBarMenu WRITE setMenuBarMenu NOTIFY menuBarMenuChanged)
    Q_PROPERTY(QObject* systemPreferences READ systemPreferences WRITE setSystemPreferences)
    Q_PROPERTY(QObject* preferences READ preferences NOTIFY preferencesChanged)
    // Desktop
    Q_PROPERTY(int numberOfDesktops READ numberOfDesktops NOTIFY numberOfDesktopsChanged)
    Q_PROPERTY(int currentDesktop READ currentDesktop NOTIFY currentDesktopChanged)
    // Power
    Q_PROPERTY(bool charging READ charging NOTIFY chargingChanged)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
private:
    QVariant system_menu;
    QJSValue m_menu_bar_menu;
    // QML references.
    QObject *system_preferences_window;

    Preferences conf_file;
    struct udev *p_udev;
//    PopUpMenu *system_menu_delegate;
    MenuBar *menu_bar;
public:
    explicit Shell(QWidget *parent = nullptr);

    Q_INVOKABLE void openMenu(QObject *menu);
    Q_INVOKABLE void focusMenuItem(int64_t index);
    Q_INVOKABLE void setMenuBarMenu(QVariantMap *menu);
    Q_INVOKABLE void setPreference(QString category, QString key, QVariant val);
    Q_INVOKABLE void launchApplication(QString name);
    Q_INVOKABLE void runCommand(QString cmd);
    Q_INVOKABLE void quit();

    // inotify !NOT USED. inotify cannot watch /sys/class files.

    // udev
    void monitor_devices();
    void stop_monitoring();

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

    QObject* systemPreferences();
    void setSystemPreferences(QObject *preferences);

    Preferences* preferences();

    int numberOfDesktops();
    int currentDesktop();

    bool charging() const;

    int batteryLevel() const;

signals:
    void created();

    void registerApplicationMenu(QString menuJson);
    void registerMenuBarMenu(QString menuJson);

    void applicationMenuRegisterRequested(QString menuJson);
    void runCommandPopUpOpenRequested();
    void launchApplicationPopUpOpenRequested();

    void applicationMenuItemTriggered(QString path);
    void menuBarMenuItemTriggered(QString path);
    void menuItemTriggered(QString path);

    void confFileChanged();
    void preferenceChanged(QString category, QString key, QVariant value);

    void preferencesChanged();

    //==========================
    // Property changed signals
    //==========================
    void menuBarMenuChanged();

    void numberOfDesktopsChanged();
    void currentDesktopChanged();

    void chargingChanged();

    void batteryLevelChanged();

public slots:
    void show();

    void onConfFileChanged();
    void onPreferenceChanged(QString category, QString key, QVariant value);

protected:
    bool event(QEvent *event) override;
};

#define LA_SHELL_INOTIFY_LIST_NUM 2
#define LA_SHELL_INOTIFY_FILE_CHARGING "/sys/class/power_supply/BAT0/status"
#define LA_SHELL_INOTIFY_FILE_BATTERY_LEVEL "/sys/class/power_supply/BAT0/capacity"
extern const char* inotify_list[LA_SHELL_INOTIFY_LIST_NUM];
void _init_inotify_list();

} // namespace la
#endif // _LA_SHELL_H
