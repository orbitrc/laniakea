#ifndef _LA_SHELL_H
#define _LA_SHELL_H

#include <QObject>
#include <QJSValue>
#include <QVariantMap>

#include <libudev.h>

#include "Preferences.h"
#include "NetworkManager.h"
#include "SystemInformation.h"

namespace la {

class Shell : public QObject
{
    Q_OBJECT

//    Q_PROPERTY(QVariant systemMenu READ systemMenu WRITE setSystemMenu)
    Q_PROPERTY(QJSValue menuBarMenu READ menuBarMenu WRITE setMenuBarMenu NOTIFY menuBarMenuChanged)
    Q_PROPERTY(QObject* systemPreferences READ systemPreferences WRITE setSystemPreferences)
    Q_PROPERTY(Preferences* preferences READ preferences NOTIFY preferencesChanged)
    Q_PROPERTY(NetworkManager* networkManager READ networkManager CONSTANT)
    Q_PROPERTY(SystemInformation* systemInformation READ systemInformation CONSTANT)
    // Desktop
    Q_PROPERTY(int numberOfDesktops READ numberOfDesktops NOTIFY numberOfDesktopsChanged)
    Q_PROPERTY(int currentDesktop READ currentDesktop NOTIFY currentDesktopChanged)
    Q_PROPERTY(QString wallpaper READ wallpaper NOTIFY wallpaperChanged)
    // Power
    Q_PROPERTY(bool charging READ charging NOTIFY chargingChanged)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    // Menus
    Q_PROPERTY(QVariantMap systemMenu READ systemMenu)

private:
    QVariant system_menu;
    QJSValue m_menu_bar_menu;
    // QML references.
    QObject *system_preferences_window;

    Preferences *m_preferences;
    NetworkManager *m_networkManager;
    SystemInformation *m_systemInformation;
    struct udev *p_udev;
public:
    explicit Shell(QObject *parent = nullptr);
    ~Shell();

    Q_INVOKABLE void openMenu(QObject *menu);
    Q_INVOKABLE QString desktopName(int desktop);
    Q_INVOKABLE void setPreference(QString category, QString key, QVariant val);
    Q_INVOKABLE void launchApplication(QString name);
    Q_INVOKABLE void runCommand(QString cmd);
    Q_INVOKABLE void quit();

    // inotify !NOT USED. inotify cannot watch /sys/class files.

    // udev
    void monitor_devices();
    void stop_monitoring();

    // Getters

    void openRebusMenu(QVariantMap *menu);

    //=================
    // Properties
    //=================
//    QVariant systemMenu();
    void setSystemMenu(QVariant menu);

    QJSValue& menuBarMenu();
    void setMenuBarMenu(QJSValue& menuBarMenu);

    QObject* systemPreferences();
    void setSystemPreferences(QObject *preferences);

    Preferences* preferences();
    NetworkManager* networkManager();
    SystemInformation* systemInformation() const;

    int numberOfDesktops();
    int currentDesktop();
    QString wallpaper() const;

    bool charging() const;

    int batteryLevel() const;

    QVariantMap systemMenu() const;

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

    void preferencesChanged();

    //==========================
    // Property changed signals
    //==========================
    void menuBarMenuChanged();

    void numberOfDesktopsChanged(int num);
    void currentDesktopChanged(int desktop);
    void wallpaperChanged(QString path);

    void chargingChanged();

    void batteryLevelChanged();

public slots:

protected:
    // bool event(QEvent *event) override;
};


class Menus {
public:
    static QVariantMap system_menu;
    static void init_system_menu();
};

} // namespace la
#endif // _LA_SHELL_H
