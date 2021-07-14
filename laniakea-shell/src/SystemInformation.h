#ifndef _LA_SYSTEM_INFORMATION_H
#define _LA_SYSTEM_INFORMATION_H

#include <QObject>

namespace la {

class LsbRelease {
public:
    QString lsbVersion;
    QString distributorId;
    QString description;
    QString release;
    QString codename;
};

class SystemInformation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString distName READ distName CONSTANT)
    Q_PROPERTY(QString distIcon READ distIcon CONSTANT)
    Q_PROPERTY(QString kernel READ kernel CONSTANT)
public:
    explicit SystemInformation(QObject *parent = nullptr);

    QString distName() const;
    QString distIcon() const;
    QString kernel() const;

private:
    bool has_lsb_release() const;
    LsbRelease lsb_release() const;

signals:

};

} // namespace la

#endif // _LA_SYSTEM_INFORMATION_H
