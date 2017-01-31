BASEDIR = ../../../../../..
CLEBS += filterplugin buildplugin syllib

include ($$BASEDIR/clebs.pri)

TARGET = firfilter

SOURCES +=                                              \
        hrtffilter.cpp					\
