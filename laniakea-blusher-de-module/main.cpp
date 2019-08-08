#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.addImportPath("/usr/share/laniakea/laniakea-blusher-de-module");
    engine.addImportPath("/usr/lib/blusher/qml");

    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
