#include "ConfFile.h"

#include <unistd.h>
#include <sys/inotify.h>
#include <string.h>
#include <stdio.h>

#include <QFile>
#include <QString>
#include <QMap>
#include <QVariant>

#include "global.h"

namespace la {

struct Preferences::Impl {
    char *conf_path;
    int inotify_fd;
    int inotify_wd;
    bool inotify_watching;
    QVariantMap conf_dict;
    Preferences::Desktop *desktop;
    Preferences::Keyboard *keyboard;
};

Preferences::Preferences()
{
    this->pImpl = new Preferences::Impl;

    // Set file path.
    this->impl().conf_path = new char[512];
    const char *home = getenv("HOME");
    sprintf(this->impl().conf_path, "%s/.config/laniakea.conf", home);

    // Initialize inotify variables.
    this->impl().inotify_fd = -1;
    this->impl().inotify_wd = -1;
    this->impl().inotify_watching = false;

    // Create categories.
    this->impl().desktop = new Desktop(this);
    emit this->desktopChanged();
    this->impl().keyboard = new Keyboard(this);
    emit this->keyboardChanged();

    this->read_conf_file();
    this->sync_with_file();
}

Preferences::~Preferences()
{
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
                emit shell->confFileChanged();
            } else if (evt->mask & IN_IGNORED){
                this->sync_with_file();
                emit shell->confFileChanged();

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
            this->impl().conf_dict.insert(line, QVariantMap());
        }
        if (line.startsWith("number_of_desktops")) {
            QStringList qv = line.split("=");
            this->impl().conf_dict["desktop"].toMap().insert("number_of_desktops", qv[1]);
        }
    }
}

void Preferences::read_conf_file()
{
    QFile f(this->impl().conf_path);
    if (!f.exists()) {
        fprintf(stderr, "%s\n", f.fileName().toStdString().c_str());
        fprintf(stderr, "ConfFile::read_conf_file - File not exitsts!\n");
        return;
    }
    f.open(QFile::ReadOnly | QFile::Text);
    QString data = f.readAll();
    f.close();

    QVariantMap conf = this->parse_conf_file(data);

    // [desktop]
    Desktop *desktop = this->impl().desktop;
    desktop->setNumberOfDesktops(conf["desktop"].toMap()["number_of_desktops"].toInt());
    // [keyboard]
    Keyboard *keyboard = this->impl().keyboard;
    keyboard->setDelayUntilRepeat(conf["keyboard"].toMap()["delay_until_repeat"].toInt());
    keyboard->setKeyRepeat(conf["keyboard"].toMap()["key_repeat"].toInt());
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


void Preferences::set_preference(const char *category, const char *key, QVariant value)
{
    QVariant original_value = this->impl().conf_dict[category].toMap()[key];
    if (original_value != value) {
        this->impl().conf_dict[category].toMap()[key] = value;
        emit shell->preferenceChanged(category, key, value);
    }
}


QVariant Preferences::get_preference(const char *category, const char *key)
{
    const QVariantMap& category_dict = this->impl().conf_dict[category].toMap();
    QVariant val = category_dict.value(key);

    return val;
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
    const QVariantMap& desktop = const_cast<Preferences*>(this)->impl().conf_dict.value("desktop").toMap();
    return desktop.value("number_of_desktops").toInt();
}







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
    emit this->numberOfDesktopsChanged(val);
    if (shell != nullptr) {
        emit shell->preferenceChanged("desktop", "number_of_desktops", val);
    }
}






Preferences::Keyboard::Keyboard(QObject *parent)
    : QObject(parent)
{
}

Preferences::Keyboard::~Keyboard()
{
}

int Preferences::Keyboard::delayUntilRepeat() const
{
    return this->m_delay_until_repeat;
}

void Preferences::Keyboard::setDelayUntilRepeat(int val)
{
    this->m_delay_until_repeat = val;
    emit this->delayUntilRepeatChanged(val);
    if (shell != nullptr) {
        emit shell->preferenceChanged("keyboard", "delay_until_repeat", val);
    }
}

int Preferences::Keyboard::keyRepeat() const
{
    return this->m_key_repeat;
}

void Preferences::Keyboard::setKeyRepeat(int val)
{
    this->m_key_repeat = val;
    emit this->keyRepeatChanged(val);
    if (shell != nullptr) {
        emit shell->preferenceChanged("keyboard", "key_repeat", val);
    }
}

} // namespace la
