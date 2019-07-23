#ifndef REBUSLISTENER_H
#define REBUSLISTENER_H

#include <QObject>

class RebusListener : public QObject
{
    Q_OBJECT
public:

signals:

public slots:
    Q_SCRIPTABLE QString request(const QString &data);
};

#endif // REBUSLISTENER_H
