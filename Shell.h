#ifndef SHELL_H
#define SHELL_H

#include <QObject>
#include <QWidget>

namespace la {

class Shell : public QWidget
{
    Q_OBJECT
public:
    explicit Shell(QWidget *parent = nullptr);

signals:

public slots:
};

} // namespace la
#endif // SHELL_H
