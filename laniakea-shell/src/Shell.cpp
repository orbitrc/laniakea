#include "Shell.h"

// Qt
#include <QApplication>
#include <QScreen>
#include <QEvent>
#include <QWindow>
#include <QThread>
#include <QShortcut>

// KDE Frameworks
#include <kwindowsystem.h>

// Laniakea
#include "global.h"

// C/C++
#include <stdlib.h>
#include <string.h>
#include <iostream>

// Unix
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>

// libudev
#include <libudev.h>

namespace la {

Shell::Shell(QObject *parent)
    : QObject(parent)
{
    /*
    MenuItemDelegate *about_system = new MenuItemDelegate(nullptr);
    MenuItemDelegate *shutdown = new MenuItemDelegate(nullptr);
    about_system->set_menu_item_title("About Laniakea");
    shutdown->set_menu_item_title("Shutdown");
    */
//    this->system_menu_delegate->add_item_delegate(about_system);
//    this->system_menu_delegate->add_item_delegate(shutdown);

    this->m_preferences = new Preferences;
    this->m_networkManager = new NetworkManager;

    //=================
    // Set properties
    //=================
    this->m_menu_bar_menu = QJSValue::NullValue;

    //=================
    // Connect signals
    //=================
    // KWindowSystem
    KWindowSystem *kWindowSystem = KWindowSystem::self();
    QObject::connect(kWindowSystem, &KWindowSystem::numberOfDesktopsChanged,
                     this, &Shell::numberOfDesktopsChanged);
    QObject::connect(kWindowSystem, &KWindowSystem::currentDesktopChanged,
                     this, &Shell::currentDesktopChanged);
    // Shell
    QObject::connect(this, &Shell::confFileChanged,
                     this, &Shell::onConfFileChanged);
    QObject::connect(this, &Shell::preferenceChanged,
                     this, &Shell::onPreferenceChanged);
    // Belows will connected in QML.


    emit this->preferencesChanged();


    // Worker threads
    QThread *thr = QThread::create([this]() {
        this->monitor_devices();
    });
    thr->start();

    QThread *thr_inotify = QThread::create([this]() {
        this->m_preferences->run_watch_loop();
    });
    thr_inotify->start();

    // DEBUG
    QObject::connect(kWindowSystem, static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>(&KWindowSystem::windowChanged),
                     this, [](WId w_id, NET::Properties props, NET::Properties2 props2) {
        (void)w_id;
        (void)props;
        (void)props2;
//        fprintf(stderr, "WId: %d\n", w_id);
//        fprintf(stderr, "WMGeometry: %d", static_cast<bool>(props | NET::WMGeometry));
//        fprintf(stderr, "\n");
    });

    /*
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Alt, Qt::Key_T), parent);
    shortcut->setContext(Qt::ApplicationShortcut);
    QObject::connect(shortcut, &QShortcut::activated,
                     this, []() { fprintf(stderr, "pressed\n"); });
    */
}

Shell::~Shell()
{
}

void Shell::monitor_devices()
{
    this->p_udev = udev_new();
    if (!this->p_udev) {
        fprintf(stderr, "Shell::monitor_devices() - udev_new() failed!\n");
    }
    struct udev_enumerate* enumerate = udev_enumerate_new(this->p_udev);
    udev_enumerate_add_match_subsystem(enumerate, "power_supply");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry* entry;

    struct udev_device *dev;
    udev_list_entry_foreach(entry, devices) {
        const char* path = udev_list_entry_get_name(entry);
//        fprintf(stderr, "path: %s\n", path);
        dev = udev_device_new_from_syspath(p_udev, path);
        const char *sysname = udev_device_get_sysname(dev);
        fprintf(stderr, "sysname: %s\n", sysname);
        if (strcmp(sysname, "BAT0") == 0) {
            const char *status = udev_device_get_property_value(dev, "POWER_SUPPLY_STATUS");
            fprintf(stderr, "%s\n", status);
        }
        udev_device_unref(dev);
    }
    udev_enumerate_unref(enumerate);

    while (1) {
        struct udev_monitor *monitor = udev_monitor_new_from_netlink(p_udev, "udev");
        udev_monitor_filter_add_match_subsystem_devtype(monitor, "power_supply", NULL);
        udev_monitor_enable_receiving(monitor);
        int monitor_fd = udev_monitor_get_fd(monitor);
        // Set to blocking mode.
        int flags = fcntl(monitor_fd, F_GETFL, 0);
        flags &= ~O_NONBLOCK;
        fcntl(monitor_fd, F_SETFL, flags);

        dev = udev_monitor_receive_device(monitor);
        const char *sysname = udev_device_get_sysname(dev);
        if (strcmp(sysname, "BAT0") == 0) {
//            const char *capacity = udev_device_get_property_value(dev, "POWER_SUPPLY_CAPACITY");
//            fprintf(stderr, "capacity: <%s>\n", capacity);
            emit this->batteryLevelChanged();
        } else if (strcmp(sysname, "AC") == 0) {
            emit this->chargingChanged();
        }
        udev_device_unref(dev);
    }
}


void Shell::stop_monitoring()
{
    udev_unref(this->p_udev);
}

void Shell::openMenu(QObject *menu)
{
    qDebug("Shell::openMenu");
    QVariant menu_type = menu->property("type");
    QVariant menu_title = menu->property("title");
    QVariant menu_items = menu->property("items");

//    PopUpMenu *m = new PopUpMenu(menu);
//    m->addAction("please...");
//    m->popup(QPoint(50, 50));
//    PopUpMenu *subm = new PopUpMenu(menu);
//    m->addMenu(subm);
//    QVariant title = menu->property("title");
//    qDebug(title.toString().toLocal8Bit());
}

void Shell::openRebusMenu(QVariantMap *menu)
{
    if ((*menu)["type"] == 0) {
        // TODO
    }
}


void Shell::setPreference(QString category, QString key, QVariant val)
{
    this->m_preferences->set_preference(category.toLocal8Bit(), key.toLocal8Bit(), val);
}

//=========================
// QML callable functions
//=========================
void Shell::quit()
{
    // Remove inotify watching file descriptors.
//    this->remove_watch_files();

    this->stop_monitoring();

    qApp->quit();
}

void Shell::launchApplication(QString name)
{
    fprintf(stderr, "launchApplication: %s\n", name.toStdString().c_str());
}

void Shell::runCommand(QString cmd)
{
    FILE *f = popen(cmd.toLocal8Bit(), "r");
    fprintf(stderr, "cmd = %s, f = %p\n", cmd.toStdString().c_str(), f);
}

QString Shell::desktopName(int desktop)
{
    return KWindowSystem::desktopName(desktop);
}


//==================
// Signal handlers
//==================
void Shell::onConfFileChanged()
{
    int number_of_desktops = this->m_preferences->number_of_desktops();
// x11
    if (qApp->platformName() == "xcb") {
        QString cmd = "wmctrl -n " + QString::number(number_of_desktops);
        fprintf(stderr, "system(%s)\n", cmd.toStdString().c_str());
        system(cmd.toLocal8Bit());
    }
// wayland
    if (qApp->platformName() == "wayland") {
        // TODO: Implement.
    }
}

void Shell::onPreferenceChanged(QString category, QString key, QVariant value)
{
    if (category == "desktop") {
        // [desktop]
        if (key == "number_of_desktops") {
// x11
            if (qApp->platformName() == "xcb") {
                QString cmd = "wmctrl -n " + value.toString();
                fprintf(stderr, "system(%s)\n", cmd.toStdString().c_str());
                system(cmd.toLocal8Bit());
            }
// wayland
            if (qApp->platformName() == "wayland") {
                // TODO: Implement.
            }
        }
    } else if (category == "keyboard") {
        // [keyboard]
        if (key == "delay_until_repeat") {
            QVariant repeat = this->m_preferences->get_preference("keyboard", "key_repeat");
            QString cmd = "xset r rate " + value.toString() + " " + repeat.toString();
            system(cmd.toLocal8Bit());
        } else if (key == "key_repeat") {
            QVariant delay = this->m_preferences->get_preference("keyboard", "delay_until_repeat");
            QString cmd = "xset r rate " + delay.toString() + " " + value.toString();
            system(cmd.toLocal8Bit());
        }
    }
}


// Getters

//=================
// Event handlers
//=================

//================
// Properties
//================

void Shell::setSystemMenu(QVariant menu)
{
    this->system_menu = menu;
//    this->system_menu_delegate->setMenu(qvariant_cast<QObject*>(menu));
}

QJSValue& Shell::menuBarMenu()
{
    return this->m_menu_bar_menu;
}

void Shell::setMenuBarMenu(QJSValue& menuBarMenu)
{
    this->m_menu_bar_menu = menuBarMenu;
    emit this->menuBarMenuChanged();
}


QObject* Shell::systemPreferences()
{
    return this->system_preferences_window;
}

void Shell::setSystemPreferences(QObject *preferences)
{
    this->system_preferences_window = preferences;
}

Preferences* Shell::preferences()
{
    return this->m_preferences;
}

NetworkManager* Shell::networkManager()
{
    return this->m_networkManager;
}

int Shell::numberOfDesktops()
{
    return KWindowSystem::numberOfDesktops();
}

int Shell::currentDesktop()
{
    return KWindowSystem::currentDesktop();
}

QString Shell::wallpaper() const
{
    auto wallpaper_path = this->m_preferences->desktop()->wallpaper();
    return wallpaper_path;
}

bool Shell::charging() const
{
    bool is_charging = false;
    struct udev *p_udev = udev_new();

    if (!p_udev) {
        fprintf(stderr, "Shell::charging() - udev_new() failed!\n");
        return false;
    }
    struct udev_enumerate* enumerate = udev_enumerate_new(p_udev);
    udev_enumerate_add_match_subsystem(enumerate, "power_supply");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry* entry;

    struct udev_device *dev;
    udev_list_entry_foreach(entry, devices) {
        const char* path = udev_list_entry_get_name(entry);
        dev = udev_device_new_from_syspath(p_udev, path);
        const char *sysname = udev_device_get_sysname(dev);
        if (strcmp(sysname, "AC") == 0) {
            const char *online = udev_device_get_property_value(dev, "POWER_SUPPLY_ONLINE");
            if (strcmp(online, "1") == 0) {
                is_charging = true;
            }
        }
        udev_device_unref(dev);
    }
    udev_enumerate_unref(enumerate);

    return is_charging;
}

int Shell::batteryLevel() const
{
    QFile f("/sys/class/power_supply/BAT0/capacity");
    f.open(QFile::ReadOnly | QFile::Text);
    QByteArray data = f.readAll();
    f.close();
    return data.toInt();
}

QVariantMap Shell::systemMenu() const
{
    if (!Menus::system_menu.contains("items")) {
        Menus::init_system_menu();
    }
    return Menus::system_menu;
}


//==============
// Menus class
//==============
QVariantMap Menus::system_menu;

void Menus::init_system_menu()
{
    QVariantList system_menu_items;
    QVariantMap menu_item;
    menu_item.insert("path", "/preferences");
    menu_item.insert("title", "System Preferences");
    system_menu_items.push_back(menu_item);

    menu_item = QVariantMap();
    menu_item.insert("path", "/separator1");
    menu_item.insert("title", "-------");
    menu_item.insert("separator", true);
    system_menu_items.push_back(menu_item);

    menu_item = QVariantMap();
    menu_item.insert("path", "/debug-quit'");
    menu_item.insert("title", "(DEBUG) Quit Laniakea Shell");
    system_menu_items.push_back(menu_item);

    Menus::system_menu.insert("items", system_menu_items);
}

} // namespace la
