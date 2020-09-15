#ifndef REBUS_LISTENER_H
#define REBUS_LISTENER_H

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

    /// @brief Start listening upcomming messages.
    void listen();

signals:
    void response(const char *data);

public slots:
    int16_t post_host();
    int16_t delete_host();

    void onNewConnection();
};

//===================
// ReBus API
//===================
struct httproto_protocol;

class Routes {
public:
    // [GET] /ping
    static void ping(const httproto_protocol *request, QLocalSocket *conection);

    // [POST] /quit
    static void quit(const httproto_protocol *request, QLocalSocket *connection);

    // [PUT] /menu/application
    // [GET] /menu/application
    static void menuApplication(const httproto_protocol *request, QLocalSocket *connection);

    // [PUT] /menu/menu-bar
    // [GET] /menu/menu-bar
    static void menuMenuBar(const httproto_protocol *request, QLocalSocket *connection);

    // [PUT] /menu-bar/application-menu
    //  argument: object
    //  e.g. {
    //    "title": string
    //    "items": list
    //  }
    class MenuBar {
    public:
        static void applicationMenu(const httproto_protocol *request, QLocalSocket *connection);
    };

    class Shell {
    public:
        // [POST] /shell/launch-application
//        static void launchApplication(const httproto_protocol *request, QLocalSocket *connection);

        // [POST] /shell/run-command
         static void runCommand(const httproto_protocol *request, QLocalSocket *connection);
    };
};

#endif // REBUS_LISTENER_H
