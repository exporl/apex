BASEDIR = ../../../../../..
CLEBS += filterplugin buildplugin syllib

include ($$BASEDIR/clebs.pri)

TARGET = threadedhrtffilter

SOURCES +=                                              \
        threadedhrtffilter.cpp				\
