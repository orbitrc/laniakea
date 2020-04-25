#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

//#include <libnm/NetworkManager.h>   // This should go before include Qt
                                    // because crash of keyword `signals`.

#include <QObject>

#include <QQmlListProperty>

namespace la {

//====================
// Class AccessPoint
//====================
class AccessPoint : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString ssid READ ssid NOTIFY ssidChanged)
    Q_PROPERTY(QString uuid READ uuid NOTIFY uuidChanged)

public:
    explicit AccessPoint(QObject *parent = nullptr);

    QString ssid() const;
    void setSsid(QString ssid);

    QString uuid() const;
    void setUuid(QString uuid);

signals:
    void ssidChanged();
    void uuidChanged();

private:
    QString m_ssid;
    QString m_uuid;
};


//=======================
// Class NetworkManager
//=======================
class NetworkManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString currentConnectionId READ currentConnectionId NOTIFY currentConnectionIdChanged)
    Q_PROPERTY(bool ethernetAvailable READ ethernetAvailable NOTIFY ethernetAvailableChanged)
    Q_PROPERTY(bool wifiAvailable READ wifiAvailable NOTIFY wifiAvailableChanged)
    Q_PROPERTY(QQmlListProperty<la::AccessPoint> knownAps READ knownAps NOTIFY knownApsChanged)

private:
    struct Impl;

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    QString currentConnectionId() const;
    bool ethernetAvailable() const;
    bool wifiAvailable() const;
    QQmlListProperty<AccessPoint> knownAps();

    Impl *impl();

    void event_loop();

signals:
    void currentConnectionIdChanged();
    void ethernetAvailableChanged();
    void wifiAvailableChanged();
    void knownApsChanged();

private:
    QString m_currentConnectionId;

    void *pImpl;
};

} // namespace la

#endif // NETWORKMANAGER_H
