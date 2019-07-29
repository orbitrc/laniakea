#include "BlusherWidget.h"

#include <QQmlEngine>
#include <QQmlContext>

#include "global.h"
#include "Shell.h"

BlusherWidget::BlusherWidget(QQuickWidget *parent)
    : QQuickWidget(parent)
{
    this->engine()->addImportPath(BLUSHER_PATH);
    this->engine()->rootContext()->setContextProperty("Process", *la::process);
    this->engine()->rootContext()->setContextProperty("Shell", QVariant::fromValue(la::shell));
}

BlusherWidget::BlusherWidget(QQmlEngine *engine, QQuickWidget *parent)
    : QQuickWidget(engine, parent)
{

}
