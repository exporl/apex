BASEDIR = ../../..
CLEBS *= apextools
TARGET = toolstest
TEMPLATE = app
include($$BASEDIR/clebs.pri)

CONFIG *= qtestlib

SOURCES *= apextoolstest.cpp

HEADERS *= apextoolstest.h
