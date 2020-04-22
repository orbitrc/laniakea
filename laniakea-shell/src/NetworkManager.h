#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

//#include <libnm/NetworkManager.h>   // This should go before include Qt
                                    // because crash of keyword `signals`.

#include <QObject>

namespace la {

class NetworkManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentConnectionId READ currentConnectionId NOTIFY currentConnectionIdChanged)

private:
    struct Impl;

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    QString currentConnectionId() const;

    Impl *impl();

signals:
    void currentConnectionIdChanged();

private:
    QString m_currentConnectionId;

    void *pImpl;
};

} // namespace la

#endif // NETWORKMANAGER_H
