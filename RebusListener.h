#ifndef REBUSLISTENER_H
#define REBUSLISTENER_H

#include <QObject>
#include <QLocalServer>

#define REBUS_HTTP_CONTENT_TYPE_JSON "application/json"

class RebusListener : public QObject
{
    Q_OBJECT
private:
    int32_t socket_fd;
    char id[37];
    QLocalServer socket;
public:
    RebusListener();
    ~RebusListener();

    void listen();

signals:
    void response(const char *data);

public slots:
    int16_t post_host();
    int16_t delete_host();

    void onNewConnection();
};

#endif // REBUSLISTENER_H
