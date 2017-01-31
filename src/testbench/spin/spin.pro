BASEDIR = ../../..
CLEBS *= buildplugin apextools apexdata apexmain
TARGET = test_spin
include($$BASEDIR/clebs.pri)

# WTF? TODO FIXME path???
INCLUDEPATH *= .. ../../plugins/spin
LIBS *= -lspin

QT *= testlib

SOURCES *= \
    spintest.cpp \

HEADERS *= \
    spintest.h \


