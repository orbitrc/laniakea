#ifndef SHELL_H
#define SHELL_H

#include <QObject>
#include <QWidget>
#include <QJSValue>
#include <QVariantMap>

namespace la {

class Shell : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QJSValue menuBarMenu READ menuBarMenu WRITE setMenuBarMenu NOTIFY menuBarMenuChanged)
private:
    QJSValue m_menu_bar_menu;
public:
    explicit Shell(QWidget *parent = nullptr);

    Q_INVOKABLE void openMenu(QObject *menu);
    Q_INVOKABLE void focusMenuItem(int64_t index);
    Q_INVOKABLE void setMenuBarMenu(QVariantMap *menu);
    Q_INVOKABLE void quit();

    void openRebusMenu(QVariantMap *menu);

    //=================
    // Properties
    //=================
    QJSValue& menuBarMenu();
    void setMenuBarMenu(QJSValue& menuBarMenu);

signals:
    void created();

    void menuBarMenuChanged();

    void registerMenuBarMenu(QString menuJson);

public slots:
    void show();

protected:
    bool event(QEvent *event) override;
};

} // namespace la
#endif // SHELL_H
