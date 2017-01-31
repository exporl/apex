BASEDIR = ../../..
CLEBS *= apextools apexdata apexmain
TARGET = spintest
TEMPLATE = app
include($$BASEDIR/clebs.pri)

CONFIG *= qtestlib

#TODO FIXME path???
INCLUDEPATH *= .. ../../plugins/spin
LIBS *= -lspin

QT *= testlib gui

SOURCES *= \
    spintest.cpp \

HEADERS *= \
    spintest.h \
