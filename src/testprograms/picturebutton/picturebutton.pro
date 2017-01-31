TEMPLATE = app
BASEDIR = ../../..
TARGET=picturebutton

include ($$BASEDIR/clebs.pri)

CONFIG += qt
QT += gui


# WTF? TODO FIXME path???
INCLUDEPATH += $$BASEDIR/src/lib/apexmain/screen

# WTF? TODO FIXME path???
SOURCES += \
    picturebutton.cpp \
    $$BASEDIR/src/lib/apexmain/screen/picturebuttonwidget.cpp
HEADERS += \
    $$BASEDIR/src/lib/apexmain/screen/picturebuttonwidget.h

