#include "RebusListener.h"

#include "global.h"
#include "Shell.h"

// Unix
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

// C
#include <stdio.h>

// Qt
#include <QApplication>
#include <QVariant>
#include <QLocalSocket>

// 3rd parties
#include <json-c/json.h>
extern "C" {
#include <json-c/json_visit.h>
}

#include <http_parser.h>

#include <httproto/httproto.h>


//==================
// Common responses
//==================
#define VOID_OK_RESPONSE "HTTP/1.1 200 OK\r\n" \
    "Content-Type: application/json\r\n" \
    "Conetnt-Length: 0\r\n" \
    "\r\n"

#define VOID_BAD_REQUEST_RESPONSE "HTTP/1.1 400 Bad Request\r\n" \
    "Content-Type: application/json\r\n" \
    "Conetnt-Length: 0\r\n" \
    "\r\n"



//=================
// Helper classes.
//=================
class HttpProtocolBase {
public:
    enum RequestMethod {
        Get,
        Post,
        Delete,
    };

    int64_t content_length;
    QByteArray body;
private:

};

class HttpResponse {
public:
    int16_t status;
    char content_type[64];
    int64_t content_length;
    QByteArray body;
    bool valid;

    static HttpResponse parse(const QByteArray response)
    {
        HttpResponse res;
        res.status = -1;
        res.valid = false;
        if (!response.startsWith("HTTP/1.1")) {
            return res;
        }

        const char *iter = response;
        bool first_line = true;
        bool headers = false;
        bool body = false;
        for (int64_t i = 0; i < response.length(); ) {
            if (first_line && i == 0) {
                if (strncmp("HTTP/1.1 ", iter, 9) != 0) { return res; }
                iter += 9; i += 9;
            } else if (first_line && i > 0) {
                QString status_str;
                status_str += *iter; ++iter; ++i;
                status_str += *iter; ++iter; ++i;
                status_str += *iter; ++iter; ++i;
                res.status = static_cast<int16_t>(status_str.toInt());
                while (*iter != '\r' && *(iter + 1) != '\n') { ++iter; ++i; }
                iter += 2; i += 2;

                first_line = false;
                headers = true;
            } else if (headers) {
                QByteArray buffer;
                while (strncmp("\r\n\r\n", iter, 4) != 0) {
                    buffer += *iter;
                    ++iter; ++i;
                }
                iter += 4; i += 4;
                QByteArrayList header_list = buffer.split('\n');
                for (QByteArray& line: header_list) {
                    QByteArrayList kv = line.split(':');
                    QByteArray value = kv[1];
                    value.remove(0, 1);
                    if (kv[0] == QByteArray("Content-Type")) {
                        strncpy(res.content_type, value, static_cast<size_t>(value.length()));
                        res.content_type[value.length()] = '\0';
                    } else if (kv[0] == QByteArray("Content-Length")) {
                        res.content_length = atoll(value);
                    }
                }

                headers = false;
                body = true;
            } else if (body) {
                if (res.body.length() < res.content_length) {
                    res.body += *iter;
                }
                ++iter; ++i;
            }
        }

        res.valid = true;
        return res;
    }
};


class HttpRequest {
public:
    enum Method {
        Get,
        Post,
        Delete,
    };

    HttpRequest::Method method;
    QByteArray path;
    char host[64];
    const char *content_type;
    int64_t content_length;
    QByteArray body;
    bool ready;

    QByteArray temp_key;
    QByteArray temp_val;

    void setHost(const char *host)
    {
        strncpy(this->host, host, strlen(host));
        this->host[strlen(host)] = '\0';
    }

    QByteArray make() const
    {
        QByteArray req;
        // First line of header.
        switch (this->method) {
        case Get:
            req += "GET ";
            break;
        case Post:
            req += "POST ";
            break;
        case Delete:
            req += "DELETE ";
            break;
        }
        req += this->path;
        req += " HTTP/1.1\r\n";

        // Headers.
        req += QByteArray("Host: ") + this->host + "\r\n";
        if (this->method == HttpRequest::Method::Post ||
                this->method == HttpRequest::Method::Delete) {
            req += QByteArray("Content-Length: ")
                + QByteArray::number(static_cast<qint64>(this->content_length))
                + "\r\n";
        }

        // End of header.
        req += "\r\n";

        req += this->body;

        return req;
    }

    static HttpRequest parse(const char *request, size_t len)
    {
        HttpRequest req;
        req.content_length = -1;
        req.ready = false;


        http_parser parser;
        http_parser_settings parser_settings;

        http_parser_init(&parser, HTTP_REQUEST);
        parser.data = static_cast<void*>(&req);

        http_parser_settings_init(&parser_settings);
        parser_settings.on_url = HttpRequest::parser_on_url;
        parser_settings.on_header_field = HttpRequest::parser_on_header_field;
        parser_settings.on_header_value = HttpRequest::parser_on_header_value;
        parser_settings.on_body = HttpRequest::parser_on_body;

        size_t parsed = http_parser_execute(&parser, &parser_settings, request, len);
        if (parsed != len) {
            fprintf(stderr, "Parse failed. %s: %s\n",
                http_errno_name(static_cast<enum http_errno>(parser.http_errno)),
                http_errno_description(static_cast<enum http_errno>(parser.http_errno))
            );
            fprintf(stderr, "Invalid HTTP request. >%s<\n", request);
            return req;
        }

        // HTTP method.
        if (parser.method == HTTP_GET) {
            req.method = HttpRequest::Method::Get;
        } else if (parser.method == HTTP_POST) {
            req.method = HttpRequest::Method::Post;
        } else if (parser.method == HTTP_DELETE) {
            req.method = HttpRequest::Method::Delete;
        } else {
            fprintf(stderr, "Invalid HTTP method. %s\n", request);
            return req;
        }

        // Headers.
        req.content_length = parser.content_length;
        if (req.content_length == -1) { req.content_length = 0; }

        // Last assert.
        if (req.content_length != req.body.length()) {
            fprintf(stderr, "Assertion failed: Content-Length is not equal to ref.body.length\n");
            return req;
        }

        // Return.
        req.ready = true;
        return req;
    }

private:
    static int parser_on_url(http_parser *parser, const char *at, size_t len)
    {
        HttpRequest *req = (HttpRequest*)(parser->data);
        QByteArray val(at, len);
        req->path = val.split('?')[0];
        return 0;
    }
    static int parser_on_header_field(http_parser *parser, const char *at, size_t len)
    {
        HttpRequest *req = (HttpRequest*)(parser->data);
        req->temp_key = QByteArray(at, static_cast<int>(len));
        return 0;
    }
    static int parser_on_header_value(http_parser *parser, const char *at, size_t len)
    {
        HttpRequest *req = (HttpRequest*)(parser->data);
        QByteArray val(at, len);
        if (req->temp_key == "Content-Length") {
            req->content_length = val.toLongLong();
        } else if (req->temp_key == "Content-Type") {
            if (val == REBUS_HTTP_CONTENT_TYPE_JSON) {
                req->content_type = REBUS_HTTP_CONTENT_TYPE_JSON;
            } else if (req->temp_key == "Host") {
                // TODO: Other types.
                strncpy(req->host, at, len);
                req->host[len] = '\0';
            } else {
                return 1;
            }
        }
        return 0;
    }
    static int parser_on_body(http_parser *parser, const char *at, size_t len)
    {
        HttpRequest *req = (HttpRequest*)(parser->data);
        req->body = QByteArray(at, len);
        return 0;
    }
};


//==================
// Static functions
//==================
static struct sockaddr_un get_socket_address(const char *path)
{
    struct sockaddr_un addr;
    bzero(&addr, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    bcopy(path, &addr.sun_path, strlen(path));

    return addr;
}

extern "C" {
static json_c_visit_userfunc json_visit_function;

typedef struct json_c_visit_userarg {
    void *self;
    void *parent;
} json_c_visit_userarg;

int json_visit_function(
    json_object *jso,
    int flags,
    json_object *parent_jso,
    const char *jso_key,
    size_t *jso_index,
    void *userarg)
{
    json_c_visit_userarg *arg = (json_c_visit_userarg*)userarg;
    fprintf(stderr, "=================\n");
    if (jso_key != NULL) {
        json_type t = json_object_get_type(jso);
        json_type parent_t = (parent_jso) ? json_object_get_type(parent_jso) : json_type_null;
        switch (t) {
        case json_type_int:
            if (parent_t == json_type_object)
                (*(QVariantMap*)arg->self)[jso_key] = json_object_get_int(jso);
            else if (parent_t == json_type_array)
                (*(QVariantList*)arg->self).push_back(json_object_get_int(jso));
            break;
        case json_type_string:
            if (parent_t == json_type_object)
                (*(QVariantMap*)arg->self)[jso_key] = json_object_get_string(jso);
            else if (parent_t == json_type_array)
                (*(QVariantList*)arg->self).push_back(json_object_get_string(jso));
            break;
        case json_type_array:
            if (parent_t == json_type_object)
                (*(QVariantMap*)arg->self)[jso_key] = QVariantList();
            else if (parent_t == json_type_array)
                (*(QVariantList*)arg->self).push_back(QVariantList());
            if (flags != JSON_C_VISIT_SECOND) {
//                arg->parent = arg->qv;
//                arg->qv = (QVariantMap*)&((*(QVariantMap*)arg->qv)[jso_key]);
            }
        default:
            break;
        }
    }
    fprintf(stderr, "Key: %s\n", jso_key);
    fprintf(stderr, "jso: %p\n", jso);
    fprintf(stderr, "parent_jso: %p\n", parent_jso);
    if (jso_index) {
        fprintf(stderr, "jso_index: %d\n", (size_t)(*jso_index));
    }
    return JSON_C_VISIT_RETURN_CONTINUE;
}
} // extern "C"


QVariantMap json_object_to_q_variant_map(json_object *obj)
{
    QVariantMap qv;
    json_c_visit_userarg arg;
    arg.self = (void*)&qv;
    arg.parent = NULL;
    int err = json_c_visit(obj, 0, json_visit_function, (void*)(&arg));
    fprintf(stderr, "visited: %d\n", err);

    return qv;
}


void menu_item_triggered_callback()
{

}

//====================
// Routes
//====================
static void routes(const httproto_protocol *request, QLocalSocket *connection)
{
    HttpResponse res;
    QByteArray path = request->uri;

    if (path == "/menu/open") {
        switch (request->method) {
        case HTTPROTO_GET:
            break;
        case HTTPROTO_POST: {
            QByteArray content = request->content;
            content += "\0";
            json_object *arg = json_tokener_parse(content);
            QVariantMap menuObj;
            menuObj["type"] = 0;
            menuObj["title"] = "Foo Menu";
            la::shell->openRebusMenu(&menuObj);
            connection->write("HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: 0\r\n"
                              "\r\n");
            connection->flush();
            connection->close();
            break;
        }
        default:
            break;
        }
    } else if (path == "/menu/application") {
        Routes::menuApplication(request, connection);
    } else if (path == "/menu/menu-bar") {
        switch (request->method) {
        case HTTPROTO_GET:
            QObject::connect(la::shell, &la::Shell::menuItemTriggered, menu_item_triggered_callback);

            connection->write("HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Conetnt-Length: 1\r\n"
                              "\r\n"
                              "0");
            break;
        case HTTPROTO_POST: {
            QByteArray content = request->content;
            content += "\0";
            json_object *json_menu = json_tokener_parse(content);
            if (json_menu != NULL) {
                emit la::shell->registerMenuBarMenu(content);
                json_object_put(json_menu);

                connection->write(VOID_OK_RESPONSE);
            } else {
                fprintf(stderr, "JSON parse error.\n");
                connection->write(VOID_BAD_REQUEST_RESPONSE);
            }

            connection->flush();
            connection->close();
            break;
        }
        default:
            break;
        }
    } else if (path == "/menu-bar/application-menu") {
        Routes::MenuBar::applicationMenu(request, connection);
    } else if (path == "/quit") {
        Routes::quit(request, connection);
    } else {

    }
    if (connection->isOpen()) {
//        connection->close();
    }
}


//=======================
// ReBus
//=======================
class RebusServer {
private:
    QString rebus_socket_name;
public:
    RebusServer()
    {
        this->rebus_socket_name = "/tmp/rebus/" + QString::number(getuid()) + "/rebus.sock";
    }

    int32_t connect() const
    {
        int32_t sock = ::socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un addr = get_socket_address(this->rebus_socket_name.toLocal8Bit());

        int32_t res = ::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(struct sockaddr_un));
        if (res != 0) {
            fprintf(stderr, "[RebusServer::connect] connect() returned %d. errno: %d", res, errno);
            if (errno == ECONNREFUSED) {
                fprintf(stderr, " (Connection refused)");
            }
            fprintf(stderr, "\n");
            return -1;
        }
        return sock;
    }

    QByteArray request(const QByteArray& data)
    {
        char buf[1024];
        int32_t sock = this->connect();
        if (sock == -1) {
            fprintf(stderr, "[RebusServer::request] Failed to connect to ReBus server.\n");
            return "";
        }
        fprintf(stdout, ">>>>>>>>\n%s\n", static_cast<const char*>(data));
        fflush(stdout);
        ssize_t sent = ::send(sock, data, static_cast<size_t>(data.length()), 0);
        if (sent == -1) {
            fprintf(stderr, "[RebusServer::request] ReBus server not response.\n");
            return "";
        }
        ssize_t received = ::recv(sock, buf, 1024, 0);
        buf[received] = '\0';
        fprintf(stdout, "<<<<<<<<\n%s\n", buf);
        ::close(sock);
//        shutdown(sock, SHUT_RDWR);
        return buf;
    }

    const char* get_socket_name() const
    {
        return this->rebus_socket_name.toLocal8Bit();
    }
};


//========================
// Main
//========================
RebusListener::RebusListener()
    : socket_fd(-1)
{
}

RebusListener::~RebusListener()
{
    if (this->socket.isListening()) {
        this->socket.close();
    }
}

void RebusListener::listen()
{
    QString path = "/tmp/rebus/" + QString::number(getuid()) + "/" + this->id;
    QObject::connect(&this->socket, &QLocalServer::newConnection,
                     this, &RebusListener::onNewConnection);
    this->socket.listen(path);
}

int16_t RebusListener::post_host()
{
    RebusServer rebus_server;
    HttpRequest req;

    req.method = HttpRequest::Method::Post;
    req.path = "/hosts";
    req.setHost("rebus");
    req.content_type = REBUS_HTTP_CONTENT_TYPE_JSON;
    QByteArray payload = "{\"host_name\":\"laniakea\",\"singleton\":true}";
    req.content_length = payload.length();
    req.body = payload;


    QByteArray res_data = rebus_server.request(req.make());
    if (res_data.length() == 0) {
        fprintf(stderr, "[RebusListener::post_host] Request to ReBus server failed.\n");
        return -1;
    }
    HttpResponse res = HttpResponse::parse(res_data);

    if (res.status == 201) {
        json_object *val = json_tokener_parse(res.body + "\0");
        if (json_object_get_type(val) == json_type_string) {
            strcpy(this->id, json_object_get_string(val));
        } else {
            fprintf(stderr, "[RebusListener::post_host] Response body is invalid. body: <%s>",
                static_cast<const char*>(res.body));
        }
        json_object_put(val);

        this->listen();
    } else {
        fprintf(stderr, "[RebusListener::post_host] Cannot post new host to ReBus. status code: %d\n",
            res.status);
    }

    return res.status;
}

int16_t RebusListener::delete_host()
{
    RebusServer rebus;
    HttpRequest req;

    QByteArray id(this->id);
    req.method = HttpRequest::Method::Delete;
    req.path = "/hosts/laniakea/" + id;
    req.setHost("rebus");
    req.content_type = REBUS_HTTP_CONTENT_TYPE_JSON;

    req.content_length = 0;

    QByteArray res_data = rebus.request(req.make());
    return 200;
}


void RebusListener::onNewConnection()
{
    QLocalSocket *conn = this->socket.nextPendingConnection();
    if (conn->waitForReadyRead(3000) == false) {
        fprintf(stderr, "(slot) [RebusListener::onNewConnection] Waiting timeout.");
    }
    QByteArray received = conn->readAll();
    fprintf(stdout, "<<<<<<<<\n%s\n", static_cast<const char*>(received));
    fflush(stdout);
    httproto_protocol *request = httproto_protocol_create(HTTPROTO_REQUEST);
    httproto_protocol_parse(request, received, received.length());

    routes(request, conn);

//    httproto_protocol_free(request);
}

//========================
// API handlers
//========================
void Routes::quit(const httproto_protocol *request, QLocalSocket *connection)
{
    switch (request->method) {
    case HTTPROTO_POST:
        connection->write("HTTP/1.1 202 ");
        connection->write(http_status_str(HTTP_STATUS_ACCEPTED));
        connection->write("\r\n");
        connection->write("Content-Type: application/json\r\n"
                          "Content-Length: 0\r\n"
                          "\r\n");
        if (!connection->flush()) {
            fprintf(stderr, "(slot) [RebusListener::onNewConnection] Failed to write to the connected socket.\n");
        }
        connection->close();
        qApp->quit();
        break;
    default:
        connection->write(VOID_BAD_REQUEST_RESPONSE);
        connection->flush();
        connection->close();
        break;
    }
}

void Routes::menuApplication(const httproto_protocol *request, QLocalSocket *connection)
{
    switch (request->method) {
    case HTTPROTO_PUT: {
        QByteArray content = request->content;
        content += "\0";
        json_object *json_menu = json_tokener_parse(content);
        if (json_menu != NULL) {
            emit la::shell->registerApplicationMenu(content);
            json_object_put(json_menu);

            connection->write(VOID_OK_RESPONSE);
        } else {
            fprintf(stderr, "JSON parse error.\n");
            connection->write(VOID_BAD_REQUEST_RESPONSE);
        }

        connection->flush();
        connection->close();
        break;
    }
    case HTTPROTO_GET:
        break;
    default:
        connection->write(VOID_BAD_REQUEST_RESPONSE);
        connection->flush();
        connection->close();
    }
}

void Routes::MenuBar::applicationMenu(const httproto_protocol *request, QLocalSocket *connection)
{
    switch (request->method) {
    case HTTPROTO_PUT: {
        QByteArray content = request->content;
        content += "\0";
        json_object *json_menu = json_tokener_parse(content);
        if (json_menu != NULL) {
            emit la::shell->applicationMenuRegisterRequested(content);
            json_object_put(json_menu);

            connection->write(VOID_OK_RESPONSE);
        } else {
            fprintf(stderr, "JSON parse error.\n");
            connection->write(VOID_BAD_REQUEST_RESPONSE);
        }

        connection->flush();
        connection->close();
        break;
    }
    case HTTPROTO_GET: {
        QObject::connect(la::shell, &la::Shell::applicationMenuItemTriggered,
                         connection, [connection](QString path) {
            connection->write("HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n");
            connection->write("Content-Length: " + QByteArray::number(path.length()) + "\r\n");
            connection->write("\r\n");
            connection->write(path.toLocal8Bit());
            connection->flush();
            connection->close();
        });
        break;
    }
    default:
        connection->write("HTTP/1.1 405 ");
        connection->write(http_status_str(HTTP_STATUS_METHOD_NOT_ALLOWED));
        connection->write("\r\n");
        connection->write("Content-Type: application/json\r\n"
                          "Content-Length: 0\r\n"
                          "Allow: PUT\r\n"
                          "\r\n");
        connection->flush();
        connection->close();
    }
}
