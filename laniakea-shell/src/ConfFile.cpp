#include "ConfFile.h"

#include <unistd.h>
#include <sys/inotify.h>
#include <string.h>
#include <stdio.h>

#include <QFile>
#include <QString>
#include <QMap>

#include "global.h"

namespace la {

struct ConfFile::Impl {
    char *conf_path;
    int inotify_fd;
    int inotify_wd;
    bool inotify_watching;
    QMap<QString, QMap<QString, QString>> conf_dict;
};

ConfFile::ConfFile()
{
    this->pImpl = new ConfFile::Impl;

    // Set file path.
    this->impl().conf_path = new char[512];
    const char *home = getenv("HOME");
    sprintf(this->impl().conf_path, "%s/.config/laniakea.conf", home);

    // Initialize inotify variables.
    this->impl().inotify_fd = -1;
    this->impl().inotify_wd = -1;
    this->impl().inotify_watching = false;

    this->sync_with_file();
}

ConfFile::~ConfFile()
{
    delete (ConfFile::Impl*)(this->pImpl);
}

ConfFile::Impl& ConfFile::impl()
{
    return *((ConfFile::Impl*)(this->pImpl));
}

int ConfFile::run_watch_loop()
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


void ConfFile::exit_loop()
{
    this->impl().inotify_watching = false;
}


void ConfFile::sync_with_file()
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
            this->impl().conf_dict.insert(line, QMap<QString, QString>());
        }
        if (line.startsWith("number_of_desktops")) {
            QStringList qv = line.split("=");
            this->impl().conf_dict["desktop"].insert("number_of_desktops", qv[1]);
        }
    }
}

//==============
// Getters
//==============
int ConfFile::number_of_desktops() const
{
    const QMap<QString, QString>& desktop = const_cast<ConfFile*>(this)->impl().conf_dict.value("desktop");
    return desktop.value("number_of_desktops").toInt();
}

} // namespace la
