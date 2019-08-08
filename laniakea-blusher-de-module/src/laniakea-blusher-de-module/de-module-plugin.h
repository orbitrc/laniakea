#ifndef DEMODULEPLUGIN_H
#define DEMODULEPLUGIN_H

#include <QQmlExtensionPlugin>

#include <QObject>

class DeModule : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int hour READ hour WRITE setHour NOTIFY hourChanged)
private:
public:
    explicit DeModule(QObject *parent = nullptr);
    ~DeModule();

    int hour() const;
    void setHour(int);

    Q_INVOKABLE void openMenu(QObject *menu);

signals:
    void hourChanged();
};

class DeModulePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
public:
    void registerTypes(const char *uri) override;

signals:

public slots:
};

#endif // DEMODULEPLUGIN_H
