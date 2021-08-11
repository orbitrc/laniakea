#include "Preferences.h"

#include <unistd.h>
#include <sys/inotify.h>
#include <string.h>
#include <stdio.h>

#include <QApplication>
#include <QFile>
#include <QFileSystemWatcher>
#include <QDir>
#include <QString>
#include <QMap>
#include <QVariant>

#include <laniakea/preferences.h>

#include "global.h"

namespace la {

struct Preferences::Impl {
    char *conf_path;
    int inotify_fd;
    int inotify_wd;
    bool inotify_watching;
    la_preferences *preferences;
    QFileSystemWatcher watcher;
    Preferences::Desktop *desktop;
    Preferences::Keyboard *keyboard;

public:
    QString conf_file_path() const
    {
        return QString("%1/.config/laniakea.conf").arg(getenv("HOME"));
    }

    QString conf_file_dir() const
    {
        return QString("%1/.config").arg(getenv("HOME"));
    }
};

Preferences::Preferences()
{
    this->pImpl = new Preferences::Impl;
    auto& impl = this->impl();

    // Set file path.
    this->impl().conf_path = new char[512];
    const char *home = getenv("HOME");
    sprintf(this->impl().conf_path, "%s/.config/laniakea.conf", home);

    // Initialize laniakea preferences.
    this->impl().preferences = la_preferences_new();
    la_preferences_load(this->impl().preferences);

    // Setup file system watcher.
    this->impl().watcher.addPath(this->impl().conf_file_path());
    qDebug() << this->impl().watcher.files();
    QObject::connect(&(this->impl().watcher), &QFileSystemWatcher::fileChanged,
                     this, &Preferences::diff, Qt::QueuedConnection);

    // Initialize inotify variables.
    this->impl().inotify_fd = -1;
    this->impl().inotify_wd = -1;
    this->impl().inotify_watching = false;

    // Create categories.
    //
    // Desktop.
    this->impl().desktop = new Desktop(this);
    impl.desktop->setWallpaper(
        la_preferences_desktop_wallpaper(impl.preferences));
    // Keyboard
    this->impl().keyboard = new Keyboard(this);
    this->impl().keyboard->setCapsLockBehavior(
        la_preferences_keyboard_caps_lock_behavior(this->impl().preferences));
    this->impl().keyboard->setDelayUntilRepeat(
        la_preferences_keyboard_delay_until_repeat(this->impl().preferences));
    this->impl().keyboard->setKeyRepeat(
        la_preferences_keyboard_key_repeat(this->impl().preferences));

    if (this->read_conf_file() == false) {
        if (this->make_conf_file() == false) {
            fprintf(stderr, "Cannot create config file.\n");
        }
    } else {
        this->sync_with_file();
    }
}

Preferences::~Preferences()
{
    la_preferences_free(this->impl().preferences);
    delete (Preferences::Impl*)(this->pImpl);
}

Preferences::Impl& Preferences::impl()
{
    return *((Preferences::Impl*)(this->pImpl));
}

int Preferences::run_watch_loop()
{
    int fd = 0;
    struct inotify_event *evt;
    char *p_evt;
    int read_len;
    char buffer[512];
    int mask = IN_MODIFY | IN_IGNORED | IN_CLOSE_WRITE;

    this->impl().inotify_fd = inotify_init();
    if (this->impl().inotify_fd == -1) {
        // ERROR!
        fprintf(stderr, "ConfFile::run_watch_loop - Failed to init inotify.\n");
        return 1;
    }
    int wd = inotify_add_watch(this->impl().inotify_fd, this->impl().conf_path, mask);
    this->impl().inotify_wd = wd;

    this->impl().inotify_watching = true;
    while (this->impl().inotify_watching) {
        read_len = read(this->impl().inotify_fd, buffer, 512);

        for (p_evt = buffer; p_evt < buffer + read_len; ) {
            evt = (struct inotify_event*)p_evt;
            fprintf(stderr, "evt: %d\n", evt->mask);

            if (evt->mask & IN_MODIFY) {
                fprintf(stderr, "file changed!\n");
                this->sync_with_file();
            } else if (evt->mask & IN_IGNORED){
                this->sync_with_file();

                fprintf(stderr, "IN_IGNORED. re-initiallize.\n");
                inotify_rm_watch(this->impl().inotify_fd, this->impl().inotify_wd);
                close(this->impl().inotify_fd);
                this->impl().inotify_fd = inotify_init();
                this->impl().inotify_wd = inotify_add_watch(
                    this->impl().inotify_fd,
                    this->impl().conf_path, mask);
            }

            p_evt += sizeof(struct inotify_event) + evt->len;
        }
    }

    fprintf(stderr, "inotify watch end.\n");
    return 0;
}


void Preferences::exit_loop()
{
    this->impl().inotify_watching = false;
}


void Preferences::sync_with_file()
{
    QFile f(this->impl().conf_path);
    if (!f.exists()) {
        fprintf(stderr, "%s\n", f.fileName().toStdString().c_str());
        fprintf(stderr, "ConfFile::sync_with_file - File not exitsts!\n");
        return;
    }
    f.open(QFile::ReadOnly | QFile::Text);
    QString data = f.readAll();
    f.close();

    // Parse file.
    QStringList lines = data.split("\n");
    for (QString& line: lines) {
        if (line.startsWith("#")) {
            continue;
        }
        if (line.startsWith("[") && line.endsWith("]")) {
            line.remove(0, 1);
            line.chop(1);
//            this->impl().conf_dict.insert(line, QVariantMap());
        }
        if (line.startsWith("number_of_desktops")) {
            QStringList qv = line.split("=");
//            this->impl().conf_dict["desktop"].toMap().insert("number_of_desktops", qv[1]);
        }
    }
}

bool Preferences::read_conf_file()
{
    QFile f(this->impl().conf_path);
    if (!f.exists()) {
        fprintf(stderr, "%s\n", f.fileName().toStdString().c_str());
        fprintf(stderr, "ConfFile::read_conf_file - File not exitsts!\n");
        return false;
    }
    f.open(QFile::ReadOnly | QFile::Text);
    QString data = f.readAll();
    f.close();

    QVariantMap conf = this->parse_conf_file(data);

    // [desktop]
    Desktop *desktop = this->impl().desktop;
    desktop->setNumberOfDesktops(conf["desktop"].toMap()["number_of_desktops"].toInt());
    // [keyboard]
//    Keyboard *keyboard = this->impl().keyboard;
//    keyboard->setDelayUntilRepeat(conf["keyboard"].toMap()["delay_until_repeat"].toInt());
//    keyboard->setKeyRepeat(conf["keyboard"].toMap()["key_repeat"].toInt());

    return true;
}

bool Preferences::make_conf_file()
{
    bool dir_exists = false;

    QDir dir(this->impl().conf_file_dir());
    if (!dir.exists()) {
        bool result = dir.mkdir(this->impl().conf_file_dir());
        if (result == false) {
            return false;
        }
    } else {
        dir_exists = true;
    }

    if (dir_exists == true) {
        QFile f(this->impl().conf_file_path());
        bool conf_exists = f.open(QIODevice::ReadWrite);
        if (conf_exists == false) {
            if (f.write("\n") == -1) {
                f.close();
                return false;
            }
        }
        f.close();
    }

    return true;
}


QVariantMap Preferences::parse_conf_file(const QString& file_data)
{
    QVariantMap dict;
    QString section_name; // category

    QStringList lines = file_data.split("\n");
    for (QString& line: lines) {
        line = line.trimmed();
        if (line.startsWith("#") || line.length() == 0) {
            continue;
        }
        if (line.startsWith("[") && line.endsWith("]")) {
            line.remove(0, 1);
            line.chop(1);
            dict.insert(line, QVariantMap());
            section_name = line;
        } else {
            QStringList qv = line.split("=");
            QVariant number_of_desktops = qv[1].toInt();
            QVariantMap section = dict[section_name].toMap();
            section[qv[0]] = number_of_desktops;
            dict[section_name] = section;
        }
    }

    return dict;
}

void Preferences::write_conf_file()
{
    QFile f(this->impl().conf_path);
}

//==============
// Getters
//==============
Preferences::Desktop* Preferences::desktop() const
{
    return const_cast<Preferences*>(this)->impl().desktop;
}

Preferences::Keyboard* Preferences::keyboard() const
{
    return const_cast<Preferences*>(this)->impl().keyboard;
}

int Preferences::number_of_desktops() const
{
    return this->desktop()->numberOfDesktops();
}


void Preferences::diff()
{
    auto& impl = this->impl();
    if (!impl.watcher.files().contains(impl.conf_file_path())) {
        while (!impl.watcher.addPath(impl.conf_file_path())) {
            // Trying to add path.
        }
    }

    // Reload preferences.conf file.
    la_preferences_free(impl.preferences);
    impl.preferences = la_preferences_new();
    int err = la_preferences_load(impl.preferences);
    if (err != LA_FILE_ERROR_SUCCESS) {
        qDebug() << "preferences_load failed!";
        return;
    }

    //=================
    // Set changes
    //=================

    // Desktop
    auto wallpaper = la_preferences_desktop_wallpaper(impl.preferences);
    impl.desktop->setWallpaper(wallpaper);

    // Keyboard
    auto behavior = la_preferences_keyboard_caps_lock_behavior(impl.preferences);
    impl.keyboard->setCapsLockBehavior(behavior);

    auto delay = la_preferences_keyboard_delay_until_repeat(impl.preferences);
    impl.keyboard->setDelayUntilRepeat(delay);

    auto repeat = la_preferences_keyboard_key_repeat(impl.preferences);
    qDebug() << repeat;
    impl.keyboard->setKeyRepeat(repeat);
}



//============================
// Preferences::Desktop
//============================

Preferences::Desktop::Desktop(QObject *parent)
    : QObject(parent)
{

}

Preferences::Desktop::~Desktop()
{
}

int Preferences::Desktop::numberOfDesktops() const
{
    return this->m_number_of_desktops;
}

void Preferences::Desktop::setNumberOfDesktops(int val)
{
    this->m_number_of_desktops = val;

    // x11
    if (qApp->platformName() == "xcb") {
        QString cmd = "wmctrl -n " + QString::number(val);
        fprintf(stderr, "system(%s)\n", cmd.toStdString().c_str());
        system(cmd.toLocal8Bit());
    }
    // wayland
    if (qApp->platformName() == "wayland") {
        // TODO: Implement.
    }

    emit this->numberOfDesktopsChanged(val);
}

QString Preferences::Desktop::wallpaper() const
{
    return this->m_wallpaper;
}

void Preferences::Desktop::setWallpaper(const QString& path)
{
    if (this->m_wallpaper != path) {
        this->m_wallpaper = path;

        emit this->wallpaperChanged(path);
    }
}


//===========================
// Preferences::Keyboard
//===========================

Preferences::Keyboard::Keyboard(QObject *parent)
    : QObject(parent)
{
    this->m_caps_lock_behavior = LA_PREFERENCES_CAPS_LOCK_BEHAVIOR_CAPS_LOCK;
    this->m_delay_until_repeat = LA_PREFERENCES_DEFAULT_KEYBOARD_DELAY_UNTIL_REPEAT;
    this->m_key_repeat = LA_PREFERENCES_DEFAULT_KEYBOARD_KEY_REPEAT;
}

Preferences::Keyboard::~Keyboard()
{
}

QString Preferences::Keyboard::capsLockBehavior() const
{
    return this->m_caps_lock_behavior;
}

void Preferences::Keyboard::setCapsLockBehavior(const QString& behavior)
{
    if (this->m_caps_lock_behavior != behavior) {
        const char *option;
        if (behavior == "CapsLock") {
            option = "capslock";
        } else if (behavior == "Ctrl") {
            option = "ctrl_modifier";
        } else if (behavior == "Esc") {
            option = "escape";
        } else {
            fprintf(stderr, "Invalid behavior: \"%s\"\n", behavior.toStdString().c_str());
            return;
        }

        this->m_caps_lock_behavior = behavior;

        QString cmd = QString("setxkbmap -option caps:%1").arg(option);
        system("setxkbmap -option");
        system(cmd.toLocal8Bit());

        emit this->capsLockBehaviorChanged(behavior);
    }
}

int Preferences::Keyboard::delayUntilRepeat() const
{
    return this->m_delay_until_repeat;
}

void Preferences::Keyboard::setDelayUntilRepeat(int val)
{
    if (this->m_delay_until_repeat != val) {
        this->m_delay_until_repeat = val;

        auto repeat = this->m_key_repeat;
        QString cmd = QString("xset r rate %1 %2").arg(val).arg(repeat);
        system(cmd.toLocal8Bit());

        emit this->delayUntilRepeatChanged(val);
    }
}

int Preferences::Keyboard::keyRepeat() const
{
    return this->m_key_repeat;
}

void Preferences::Keyboard::setKeyRepeat(int val)
{
    if (this->m_key_repeat != val) {
        this->m_key_repeat = val;

        auto delay = this->m_delay_until_repeat;
        QString cmd = QString("xset r rate %1 %2").arg(delay).arg(val);
        qDebug() << cmd;
        system(cmd.toLocal8Bit());

        emit this->keyRepeatChanged(val);
    }
}


//========================
// Preferences::Display
//========================

Preferences::Display::Display(QObject *parent)
    : QObject(parent)
{
}

Preferences::Display::~Display()
{
}

//===========================
// Preferences::Localization
//===========================

Preferences::Localization::Localization(QObject *parent)
    : QObject(parent)
{
}

Preferences::Localization::~Localization()
{
}

} // namespace la
