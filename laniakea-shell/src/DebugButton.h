#ifndef _LA_DEBUG_BUTTON_H
#define _LA_DEBUG_BUTTON_H

#include <QWidget>

namespace la {

class DebugButton : public QWidget
{
    Q_OBJECT
private:
    bool mouseEntered;

public:
    explicit DebugButton(QWidget *parent = nullptr);

protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:
    void entered();
    void leaved();
    void clicked();

public slots:
};

} // namespace la

#endif // _LA_DEBUG_BUTTON_H
