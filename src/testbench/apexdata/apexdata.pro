BASEDIR = ../../..
CLEBS *= buildplugin apextools apexdata
TARGET = test_libdata
include($$BASEDIR/clebs.pri)

# WTF? TODO FIXME path???
INCLUDEPATH *= $$BASEDIR/src/testbench
QT *= testlib

SOURCES *= \
    apexdatatest.cpp \

HEADERS *= \
    apexdatatest.h \


