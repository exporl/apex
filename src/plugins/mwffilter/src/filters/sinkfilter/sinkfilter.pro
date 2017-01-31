BASEDIR = ../../../../../..
CLEBS += filterplugin buildplugin syllib sndfile

include ($$BASEDIR/clebs.pri)

TARGET = sinkfilter

SOURCES +=                                              \
        sinkfilter.cpp					\
