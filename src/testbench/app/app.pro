BASEDIR = ../../..
CLEBS *= apextools
TARGET = apextests
TEMPLATE = app
include($$BASEDIR/clebs.pri)
QT *= testlib gui
# WTF? TODO FIXME path???
INCLUDEPATH *= ..

SOURCES *= \
    main.cpp \
