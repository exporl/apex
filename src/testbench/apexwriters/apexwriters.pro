BASEDIR = ../../..
CLEBS *= buildplugin apexmain apextools apexdata apexwriters
TARGET = test_libwriters
include($$BASEDIR/clebs.pri)

# WTF? TODO FIXME path???
INCLUDEPATH *= ..
QT *= testlib
DEFINES *= NOSTATUS

SOURCES *= \
    apexwriterstest.cpp \

HEADERS *= \
    apexwriterstest.h \


