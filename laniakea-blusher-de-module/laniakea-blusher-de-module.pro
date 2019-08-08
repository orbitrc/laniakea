QT = gui quick widgets

DEFINES += QT_DEPRECATED_WARNINGS

TEMPLATE = lib

VERSION = 0.1.0

SOURCES += \
        src/laniakea-blusher-de-module/de-module-plugin.cpp

HEADERS += src/laniakea-blusher-de-module/de-module-plugin.h

TARGET = laniakea-blusher-de-module

DISTFILES += \
    src/laniakea-blusher-de-module/qml/DesktopEnvironmentModule/DesktopEnvironmentModule.qml

