#include "Shell.h"

namespace la {

Shell::Shell(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    this->setGeometry(0, 0, 100, 100);
}

}
