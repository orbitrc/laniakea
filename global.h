#ifndef _LA_GLOBAL_H
#define _LA_GLOBAL_H

#include <QVariantMap>
#include <QQmlEngine>

namespace la {

// Forward declaration.
class Shell;

extern la::Shell *shell;
extern QVariantMap *process;
extern QQmlEngine *engine;

} // namespace la

#endif // _LA_GLOBAL_H
