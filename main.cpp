#include <QApplication>
#include <blusher/blusher-qt.h>

#include <curl/curl.h>

#include <stdint.h>

#include "global.h"
#include "Shell.h"
#include "MenuBar.h"
#include "RebusListener.h"
#include <QMenu>
#include "PopUpMenu.h"

static QList<MenuBar*> menu_bar_list;

namespace la {
    Shell *shell = nullptr;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RebusListener rebus;

    QVariantMap process;
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

    qmlRegisterType<la::Shell>("Laniakea", 0, 1, "LaniakeaShell");
    qmlRegisterType<MenuBar>("LaniakeaShell", 0, 1, "MenuBar");

    la::Shell shell;
    la::shell = &shell;
    shell.show();

    QList<QScreen*> screens = QGuiApplication::screens();
    for (int32_t i = 0; i < screens.length(); ++i) {
        MenuBar *window = new MenuBar(process);
        menu_bar_list.append(window);
        window->moveToScreen(screens[i]);
//        window->show();
    }
    MenuBar test(process);
//    test.setParent(&shell);
    test.moveToScreen(QGuiApplication::screens()[0]);
    test.show();

    return app.exec();
}
