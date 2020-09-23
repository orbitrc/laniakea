QT += quick quickwidgets KWindowSystem
CONFIG += c++17

CONFIG += link_pkgconfig
CONFIG(debug, debug|release) {
    CONFIG += qml_debug
}

VERSION = 0.1.0

LIBS += -ludev -lcurl -ljson-c -lnm -lhttproto

INCLUDEPATH += \
        /home/hardboiled65/dev/Blusher/include

PKGCONFIG += libnm

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/NetworkManager.cpp \
        src/Preferences.cpp \
        src/RebusListener.cpp \
        src/Shell.cpp \
        src/main.cpp

RESOURCES += \
        resources.qrc

# Used to resolve QML modules in Qt Creator code model.
QML_IMPORT_PATH = \
        ./src/qml/components \
        ./src/qml/modules

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

DEFINES += "BLUSHER_APP_NAME=\\\"Laniakea\\ Shell\\\""
DEFINES += BLUSHER_APP_VERSION=\\\"$$VERSION\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/Preferences.h \
    src/global.h \
    src/NetworkManager.h \
    src/RebusListener.h \
    src/Shell.h

