#ifndef _LA_GLOBAL_H
#define _LA_GLOBAL_H

#include <QVariantMap>
#include <QQmlEngine>
#include <QQuickWidget>

#include "Shell.h"

#define LA_SHELL_VERSION "0.1.0"

namespace la {

// Forward declaration.
class Shell;

extern la::Shell *shell;
extern QVariantMap *process;
extern QQmlEngine *engine;

} // namespace la

#endif // _LA_GLOBAL_H
