#include "RebusListener.h"

QString RebusListener::request(const QString &data)
{
//    QMetaObject::invokeMethod(this, "quit");
    return QString("request(\"%1\") got!").arg(data);
}
