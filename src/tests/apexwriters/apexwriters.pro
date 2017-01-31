CLEBS *= apexmain apextools apexdata apexwriters asciicast
TARGET = apexwriterstest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib gui widgets

# TODO most source files are not used
SOURCES = apexwriterstest.cpp
HEADERS = apexwriterstest.h
