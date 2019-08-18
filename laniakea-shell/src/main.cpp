#include <QApplication>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <blusher/blusher-qt.h>

#include <curl/curl.h>

#include <stdint.h>
#include <stdio.h>

#include "global.h"
#include "Shell.h"
#include "BlusherWidget.h"
#include "RebusListener.h"
#include "PopUpMenu.h"
#include "PopUpMenuDelegate.h"

namespace la {
    Shell *shell = nullptr;
    QVariantMap *process = nullptr;
    QQmlEngine *engine = nullptr;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.addImportPath(BLUSHER_PATH);
    engine.addImportPath("qrc:/components");
    engine.addImportPath("qrc:/modules");
    la::engine = &engine;

    // Connect to ReBus server and post new host.
    RebusListener rebus;
    if (rebus.post_host() != 201) {
        return 1;
    }
    QObject::connect(&app, &QApplication::aboutToQuit,
                     &rebus, &RebusListener::delete_host, Qt::AutoConnection);

    QVariantMap process;
    la::process = &process;
    QVariantMap process_env;
    process_env.insert("BLUSHER_PATH", BLUSHER_PATH);
    process_env.insert("BLUSHER_DE_MODULE_PATH", "");
    process_env.insert("BLUSHER_APP_NAME", BLUSHER_APP_NAME);
    process_env.insert("BLUSHER_APP_VERSION", BLUSHER_APP_VERSION);
#ifdef BLUSHER_DEBUG
    process_env.insert("BLUSHER_DEBUG", true);
#endif
    process.insert("env", process_env);
    process.insert("app", QVariant::fromValue(&app));

    engine.rootContext()->setContextProperty("Process", process);

    la::Shell shell;
    la::shell = &shell;

    shell.show();

    engine.rootContext()->setContextProperty("Shell", QVariant::fromValue(&shell));

    qmlRegisterType<la::PopUpMenuDelegate>("LaniakeaShell", 0, 1, "PopUpMenuDelegate");


    QList<QScreen*> screens = QGuiApplication::screens();

    QObject::connect(&app, &QGuiApplication::screenAdded,
                     &app, [](QScreen *screen) {
        fprintf(stderr, "screen added!\n");
    });
    QObject::connect(&app, &QGuiApplication::screenRemoved,
                     &app, [](QScreen *screen) {
        fprintf(stderr, "screen removed!\n");
    });

//    QObject::connect(
//        &app, &QObject::objectNameChanged,
//        &app, [app](const QString& objectName) { app.onObjectNameChanged(objectName); },
//        Qt::QueuedConnection
//    );

    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
