BASEDIR = ../../../../../..
include ($$BASEDIR/clebs.pri)

QT -= gui
CONFIG += plugin
TEMPLATE = lib
TARGET = fadefilter
SOURCES += fadefilter.cpp

# WTF? TODO FIXME path???
INCLUDEPATH += ../../../../../lib/apexmain/filter
DEPENDPATH += ../../../../../lib/apexmain/filter
