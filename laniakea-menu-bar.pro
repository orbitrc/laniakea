QT += quick quickwidgets KWindowSystem
CONFIG += c++11

BLUSHER_APP_NAME = "Laniakea Shell"

VERSION = 0.1.0

LIBS += -lcurl -ljson-c -lhttp_parser

INCLUDEPATH += ./httproto/include

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
        BlusherWidget.cpp \
        MenuBar.cpp \
        MenuBarMenuDelegate.cpp \
        MenuItemDelegate.cpp \
        PopUpMenu.cpp \
        PopUpMenuDelegate.cpp \
        RebusListener.cpp \
        Shell.cpp \
        httproto/src/httproto.c \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = \
        /usr/lib/blusher/qml \
        ./src/components \
        ./src/modules

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

DEFINES += "BLUSHER_APP_NAME=\\\"Laniakea\\ Shell\\\""
DEFINES += BLUSHER_APP_VERSION=\\\"$$VERSION\\\" \
        BLUSHER_PATH=\\\"/usr/lib/blusher/qml\\\"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    BlusherWidget.h \
    MenuBar.h \
    MenuBarMenuDelegate.h \
    MenuItemDelegate.h \
    PopUpMenu.h \
    PopUpMenuDelegate.h \
    RebusListener.h \
    Shell.h \
    global.h \
    httproto/include/httproto/httproto.h
