BASEDIR = ../../..
CLEBS *= apextools apexdata apexmain
TARGET = maintest
TEMPLATE = app
include($$BASEDIR/clebs.pri)

CONFIG *= qtestlib

QT *= testlib gui

SOURCES *= apexmaintest.cpp

HEADERS *= apexmaintest.h
