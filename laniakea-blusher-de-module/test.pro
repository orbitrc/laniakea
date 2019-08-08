QT += quick
CONFIG += c++11

VERSION = 0.1.0

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp

#RESOURCES += qml.qrc

QML_IMPORT_PATH = \
        /usr/share/laniakea/laniakea-blusher-de-module \
        /usr/lib/blusher/qml

QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#HEADERS +=

RESOURCES += \
    qml.qrc

DISTFILES += \
    share/laniakea/laniakea-blusher-de-module/DesktopEnvironmentModule/DesktopEnvironmentModule.qml \
    share/laniakea/laniakea-blusher-de-module/DesktopEnvironmentModule/qmldir
