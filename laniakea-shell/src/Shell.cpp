#include "Shell.h"

// Qt
#include <QApplication>
#include <QScreen>
#include <QEvent>
#include <QWindow>
#include <QThread>

// KDE Frameworks
#include <kwindowsystem.h>

// Laniakea
#include "global.h"
#include "PopUpMenu.h"

// C
#include <string.h>

// Unix
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>

// libudev
#include <libudev.h>

#include <iostream>

namespace la {

Shell::Shell(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    this->setGeometry(0, 0, 1, 1);

    MenuItemDelegate *about_system = new MenuItemDelegate(nullptr);
    MenuItemDelegate *shutdown = new MenuItemDelegate(nullptr);
    about_system->set_menu_item_title("About Laniakea");
    shutdown->set_menu_item_title("Shutdown");
//    this->system_menu_delegate->add_item_delegate(about_system);
//    this->system_menu_delegate->add_item_delegate(shutdown);

    // Properties
    this->m_menu_bar_menu = QJSValue::NullValue;

    // Signals
    KWindowSystem *kWindowSystem = KWindowSystem::self();
    QObject::connect(kWindowSystem, &KWindowSystem::numberOfDesktopsChanged,
                     this, [this](int) { emit this->numberOfDesktopsChanged(); });
    QObject::connect(kWindowSystem, &KWindowSystem::currentDesktopChanged,
                     this, [this](int) { emit this->currentDesktopChanged(); });
    // Belows will connected in QML.


    // TIMER TEST
    QThread *thr = QThread::create([this]() {
        this->monitor_devices();
    });
    thr->start();
}


void Shell::watch_files()
{
    int fd = 0;
    struct inotify_event *evt;
    char *p_evt;
    int read_len;
    char buffer[64];

    fd = inotify_init();
    if (fd == -1) {
        // ERROR!
    }
    for (int i = 0; i < LA_SHELL_INOTIFY_LIST_NUM; ++i) {
        this->inotify_fd_list.push_back(fd);
        int wd = inotify_add_watch(fd, inotify_list[i], IN_MODIFY);
        this->inotify_wd_list.push_back(wd);
    }

    this->inotify_watching = true;
    while (this->inotify_watching) {
        read_len = read(fd, buffer, 64);

        for (p_evt = buffer; p_evt < buffer + read_len; ) {
            evt = (struct inotify_event*)p_evt;

            if (evt->mask & IN_MODIFY && strcmp(evt->name, LA_SHELL_INOTIFY_FILE_CHARGING) == 0) {
                emit this->chargingChanged();
            } else if (evt->mask & IN_MODIFY && strcmp(evt->name, LA_SHELL_INOTIFY_FILE_BATTERY_LEVEL) == 0) {
                std::cout << this->batteryLevel() << std::endl;
                emit this->batteryLevelChanged();
            } else {
                fprintf(stderr, "inotify warning: evt = %d, name = \"%s\"", evt->mask, evt->name);
            }

            p_evt += sizeof(struct inotify_event) + evt->len;
        }
    }
}

void Shell::remove_watch_files()
{
    this->inotify_watching = false;
    for (int i = 0; i < this->inotify_fd_list.length(); ++i) {
        inotify_rm_watch(this->inotify_fd_list[i], this->inotify_wd_list[i]);
    }
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
            const char *capacity = udev_device_get_property_value(dev, "POWER_SUPPLY_CAPACITY");
            fprintf(stderr, "capacity: <%s>\n", capacity);
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

void Shell::show()
{
    this->created();
    QWidget::show();
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

void Shell::setMenuBarMenu(QVariantMap *menu)
{
    // TODO
}

void Shell::focusMenuItem(int64_t index)
{
    qDebug("focusMenuItem");
}


void Shell::quit()
{
    // Remove inotify watching file descriptors.
//    this->remove_watch_files();

    this->stop_monitoring();

    qApp->quit();
}


//==================
// Signal handlers
//==================



// Getters
PopUpMenu* Shell::systemMenuDelegate() const
{
    return nullptr; //this->system_menu_delegate;
}

//=================
// Event handlers
//=================
bool Shell::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::WinIdChange:
    {
        std::cout << "[MenuBar(" << this << ")" << "::WinIdChange]" << std::endl;
        QRect rect = this->window()->windowHandle()->screen()->geometry();
        this->setGeometry(rect.x(), rect.y(), 1, 1); // (0, 0) not strut.

        WId wid = effectiveWinId();
        KWindowSystem::setExtendedStrut(wid,
            0, 0, 0,
            0, 0, 0,
            30, rect.x(), rect.width(),
            0, 0, 0);
        KWindowSystem::setOnDesktop(wid, NET::OnAllDesktops);

        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}

//================
// Properties
//================
QVariant Shell::systemMenu()
{
    return this->system_menu;
}

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

int Shell::numberOfDesktops()
{
    return KWindowSystem::numberOfDesktops();
}

int Shell::currentDesktop()
{
    return KWindowSystem::currentDesktop();
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

const char* inotify_list[LA_SHELL_INOTIFY_LIST_NUM];

void _init_inotify_list()
{
    inotify_list[0] = LA_SHELL_INOTIFY_FILE_CHARGING;
    inotify_list[1] = LA_SHELL_INOTIFY_FILE_BATTERY_LEVEL;
}

} // namespace la
