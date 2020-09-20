#include <QApplication>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QScreen>

#include <blusher/blusher.h>

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
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    // Shell arguments processing.
    for (int i = 0; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "--version") {
            fprintf(stdout, "laniakea-shell %s\n", LA_SHELL_VERSION);
            return 0;
        }
    }

    bl::Application app(argc, argv);

    app.engine()->addImportPath(BLUSHER_PATH);
    app.engine()->addImportPath("qrc:/components");
    app.engine()->addImportPath("qrc:/modules");
    app.engine()->addPluginPath("/usr/lib");
    la::engine = app.engine();

    qmlRegisterUncreatableType<la::AccessPoint>("Laniakea", 0, 1, "AccessPoint", "");
    // Connect to ReBus server and post new host.
    RebusListener rebus;
    if (rebus.post_host() != 201) {
        fprintf(stderr, "Could not connect to ReBus server.\n");
        return 1;
    }
    QObject::connect(&app, &QApplication::aboutToQuit,
                     &rebus, &RebusListener::delete_host, Qt::AutoConnection);

    la::Shell shell;
    la::shell = &shell;

    shell.show();

    la::engine->rootContext()->setContextProperty("Shell", QVariant::fromValue(&shell));

    qmlRegisterType<la::PopUpMenuDelegate>("LaniakeaShell", 0, 1, "PopUpMenuDelegate");

//    QObject::connect(
//        &app, &QObject::objectNameChanged,
//        &app, [app](const QString& objectName) { app.onObjectNameChanged(objectName); },
//        Qt::QueuedConnection
//    );

    app.engine()->load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}
