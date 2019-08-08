#include "de-module-plugin.h"

#include <QQmlEngine>
#include <QApplication>
#include <QLocalSocket>

static int32_t argc = 0;

DeModule::DeModule(QObject *parent)
    : QObject(parent)
{
}

DeModule::~DeModule()
{
}

int DeModule::hour() const
{
    return 42;
}

void DeModule::setHour(int i)
{
    ++i;
}

void DeModule::openMenu(QObject *menu)
{
    QVariant title = menu->property("title");
    QLocalSocket rebus;
    rebus.connectToServer("/tmp/rebus/1000/rebus.sock");
    if (rebus.isValid()) {
        qDebug("Yes valid");
    }
    if (rebus.waitForConnected(1000)) {
        qDebug("waitForConnection OK");
    }
    rebus.write("POST /menu/open HTTP/1.1\r\nHost: laniakea\r\n\r\n");
    rebus.flush();
    if (rebus.waitForReadyRead(1000)) {
        qDebug("waitForReadyRead OK");
    }
    QByteArray recvd = rebus.readAll();
    qDebug((QString("DeModule.openMenu: ") + title.toString()).toLocal8Bit());
    qDebug(recvd);
}



// Second, define the singleton type provider function (callback).
static QObject* de_module_singleton_type_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    DeModule *example = new DeModule();
    return example;
}

void DeModulePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("DesktopEnvironmentModule"));
//    qmlRegisterType<DeModule>("DesktopEnvironmentModule", 0, 1, "DeModule");
    qmlRegisterSingletonType<DeModule>("DesktopEnvironmentModule", 0, 1,
                                       "DeModule", de_module_singleton_type_provider);
}
