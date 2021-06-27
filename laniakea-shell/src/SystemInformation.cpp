#include "SystemInformation.h"

#include <sys/utsname.h>

#include <QProcess>
#include <QDebug>

namespace la {

SystemInformation::SystemInformation(QObject *parent) : QObject(parent)
{
}

QString SystemInformation::distName() const
{
    if (this->has_lsb_release()) {
        LsbRelease lsbRelease = this->lsb_release();
        if (lsbRelease.distributorId == "ManjaroLinux") {
            return "Manjaro";
        } else {
            return lsbRelease.distributorId;
        }
    }
    return "Linux(Unknown)";
}

QString SystemInformation::kernel() const
{
    struct utsname ret;
    if (uname(&ret) < 0) {
        return "Unknown";
    }
    return ret.release;
}

bool SystemInformation::has_lsb_release() const
{
    if (system("which lsb_release > /dev/null 2>&1") == 0) {
        return true;
    } else {
        return false;
    }
}

LsbRelease SystemInformation::lsb_release() const
{
    LsbRelease lsbRelease;

    QProcess p;
    p.start("lsb_release", QStringList("-a"));
    p.waitForFinished(1000);
    QString res = p.readAllStandardOutput();
    p.close();

    for (auto &line: res.split("\n")) {
        if (line.startsWith("LSB Version")) {
            lsbRelease.lsbVersion = line.remove("LSB Version:").trimmed();
        } else if (line.startsWith("Distributor ID")) {
            lsbRelease.distributorId = line.remove("Distributor ID:").trimmed();
        } else if (line.startsWith("Description")) {
            lsbRelease.description = line.remove("Description:").trimmed();
        } else if (line.startsWith("Release")) {
            lsbRelease.description = line.remove("Release:").trimmed();
        } else if (line.startsWith("Codename")) {
            lsbRelease.codename = line.remove("Codename:").trimmed();
        }
    }

    return lsbRelease;
}

} // namespace la
