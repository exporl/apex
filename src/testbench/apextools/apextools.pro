BASEDIR = ../../..
CLEBS *= buildplugin apextools
TARGET = test_libtools
include($$BASEDIR/clebs.pri)

# WTF? TODO FIXME path???
INCLUDEPATH *= ..
QT *= testlib

SOURCES *= \
    apextoolstest.cpp \

HEADERS *= \
    apextoolstest.h \


