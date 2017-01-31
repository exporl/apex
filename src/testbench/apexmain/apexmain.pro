BASEDIR = ../../..
CLEBS *= buildplugin apextools apexdata apexmain
TARGET = test_libapex
include($$BASEDIR/clebs.pri)

# WTF? TODO FIXME path???
INCLUDEPATH *= ..
QT *= testlib

SOURCES *= \
    apexmaintest.cpp \

HEADERS *= \
    apexmaintest.h \


