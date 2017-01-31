BASEDIR = ../../..
CLEBS *= apextools apexdata
TEMPLATE = app
TARGET = datatest
include($$BASEDIR/clebs.pri)

CONFIG *= qtestlib

SOURCES *= apexdatatest.cpp

HEADERS *= apexdatatest.h
