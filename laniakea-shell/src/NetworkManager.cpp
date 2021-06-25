#include "NetworkManager.h"

#define TEMP_SIGNALS signals
#undef signals
#include <libnm/NetworkManager.h>
#define signals TEMP_SIGNALS


namespace la {

//=======================
// Class NetworkManager
//=======================
struct NetworkManager::Impl {
    NMClient *client;
    NMDevice *current_device;
    NMDevice *ethernet_device;
    NMDevice *wifi_device;
    QList<AccessPoint*> known_ap_list;

    Impl()
    {
        this->client = NULL;
        this->current_device = NULL;
        this->ethernet_device = NULL;
        this->wifi_device = NULL;
    }

    ~Impl()
    {
    }
};

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
{
    this->pImpl = new NetworkManager::Impl;

    this->impl()->client = nm_client_new(NULL, NULL);

    const GPtrArray *devices = nm_client_get_devices(this->impl()->client);
    for (unsigned int i = 0; i < devices->len; ++i) {
        NMDevice *dev = NULL;
        dev = (NMDevice*)g_ptr_array_index(devices, i);
        fprintf(stderr, "%s:\n", nm_device_get_iface(dev));
        fprintf(stderr, " %d\n", nm_device_get_state(dev));
        fprintf(stderr, " %d\n", nm_device_get_device_type(dev));
        // Set ethernet device.
        if (nm_device_get_device_type(dev) == NM_DEVICE_TYPE_ETHERNET) {
            this->impl()->ethernet_device = dev;
            emit this->ethernetAvailableChanged();
        }
        // Set Wi-Fi device.
        if (nm_device_get_device_type(dev) == NM_DEVICE_TYPE_WIFI) {
            this->impl()->wifi_device = dev;
            emit this->wifiAvailableChanged();
        }
        if (nm_device_get_state(dev) == NM_DEVICE_STATE_ACTIVATED) {
            this->impl()->current_device = dev;
        }
    }

    if (this->impl()->current_device != NULL) {
        NMConnection *connection;
        guint64 v_id;
        connection = nm_device_get_applied_connection(
            this->impl()->current_device,
            0,
            &v_id,
            NULL,
            NULL
        );
        this->m_currentConnectionId = nm_connection_get_id(connection);
        emit this->currentConnectionIdChanged();
    }

    // DEBUG //
    // WIFI
    if (this->impl()->wifi_device != NULL) {
        const GPtrArray *aps = nm_device_wifi_get_access_points((NMDeviceWifi*)(this->impl()->wifi_device));
        for (unsigned int i = 0; i < aps->len; ++i) {
            NMAccessPoint *ap = NULL;
            ap = (NMAccessPoint*)g_ptr_array_index(aps, i);
            GBytes *ssid = nm_access_point_get_ssid(ap);
            const char *ssid_str = nm_utils_ssid_to_utf8(
                (const guint8*)g_bytes_get_data(ssid, NULL),
                g_bytes_get_size(ssid)
            );
            fprintf(stderr, "%s\n", ssid_str);
        }
        fprintf(stderr, "=====================\n");
        // Connections
        const GPtrArray *conns = nm_client_get_connections(this->impl()->client);
        for (unsigned int i = 0; i < conns->len; ++i) {
            NMConnection *conn = (NMConnection*)g_ptr_array_index(conns, i);
            if (nm_connection_is_type(conn, NM_SETTING_WIRELESS_SETTING_NAME) == TRUE) {
                AccessPoint *ap = new AccessPoint;
                ap->setSsid(nm_connection_get_id(conn));
                ap->setUuid(nm_connection_get_uuid(conn));
                this->impl()->known_ap_list.append(ap);
            }
        }
    }
}

NetworkManager::~NetworkManager()
{
    delete (NetworkManager::Impl*)(this->pImpl);
}

NetworkManager::Impl* NetworkManager::impl()
{
    return (NetworkManager::Impl*)(this->pImpl);
}

QString NetworkManager::currentConnectionId() const
{
    return this->m_currentConnectionId;
}

bool NetworkManager::ethernetAvailable() const
{
    return (const_cast<NetworkManager*>(this)->impl()->ethernet_device != NULL);
}

bool NetworkManager::wifiAvailable() const
{
    return (const_cast<NetworkManager*>(this)->impl()->wifi_device != NULL);
}

QQmlListProperty<AccessPoint> NetworkManager::knownAps()
{
    return QQmlListProperty<AccessPoint>(this, this->impl()->known_ap_list);
}

//================
// Event loop
//================
void NetworkManager::event_loop()
{
}


//====================
// Class AccessPoint
//====================
AccessPoint::AccessPoint(QObject *parent)
    : QObject(parent)
{

}

QString AccessPoint::ssid() const
{
    return this->m_ssid;
}

void AccessPoint::setSsid(QString ssid)
{
    if (this->m_ssid != ssid) {
        this->m_ssid = ssid;

        emit this->ssidChanged();
    }
}

QString AccessPoint::uuid() const
{
    return this->m_uuid;
}

void AccessPoint::setUuid(QString uuid)
{
    if (this->m_uuid != uuid) {
        this->m_uuid = uuid;

        emit this->uuidChanged();
    }
}

} // namespace la
