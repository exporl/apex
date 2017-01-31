BASEDIR = ../../..
CLEBS *= apexmain apextools apexdata apexwriters
TARGET = writerstest
TEMPLATE = app
include($$BASEDIR/clebs.pri)

CONFIG *= qtestlib 

QT *= gui

SOURCES *= apexwriterstest.cpp

HEADERS *= apexwriterstest.h
