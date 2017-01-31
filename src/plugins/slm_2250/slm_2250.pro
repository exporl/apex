BASEDIR = ../../../
CLEBS *= buildplugin \
        apextools \
        win32
include ($$BASEDIR/clebs.pri)

QT *= gui
CONFIG += qaxcontainer
TARGET = slm_2250
INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/

SOURCES *=  slm_2250.cpp \
            axbase.cpp \
            axobject.cpp \
            basicenvremoteapi.cpp


HEADERS += $$BASEDIR/src/lib/apexmain/calibration/soundlevelmeter.h \
            axbase.h \
            axobject.h \
            basicenvremoteapi.h


