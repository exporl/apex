BASEDIR =../../../..
CLEBS += filterplugin buildplugin
CLEBS *= win32
include ($$BASEDIR/clebs.pri)

QT *= gui
CONFIG += console qaxcontainer

INCLUDEPATH += $$BASEDIR/src/lib/apexmain/gui

HEADERS = \
    $$BASEDIR/src/lib/apexmain/gui/flashwidget.h \
    testobject.h

SOURCES = \
    flashtest.cpp \
    $$BASEDIR/src/lib/apexmain/gui/flashwidget.cpp \
    shockwaveflashobjects.cpp \
    testobject.cpp \

