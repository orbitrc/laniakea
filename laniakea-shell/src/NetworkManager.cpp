#include "NetworkManager.h"

#define TEMP_SIGNALS signals
#undef signals
#include <libnm/NetworkManager.h>
#define signals TEMP_SIGNALS


namespace la {

struct NetworkManager::Impl {
    NMClient *client;
    NMDevice *current_device;
};

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
{
    this->pImpl = new NetworkManager::Impl;

    // Initialize.
    this->impl()->client = NULL;
    this->impl()->current_device = NULL;

    this->impl()->client = nm_client_new(NULL, NULL);

    const GPtrArray *devices = nm_client_get_devices(this->impl()->client);
    for (unsigned int i = 0; i < devices->len; ++i) {
        NMDevice *dev = NULL;
        dev = (NMDevice*)g_ptr_array_index(devices, i);
        fprintf(stderr, "%s\n", nm_device_get_iface(dev));
        fprintf(stderr, "%d\n", nm_device_get_state(dev));
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

} // namespace la
