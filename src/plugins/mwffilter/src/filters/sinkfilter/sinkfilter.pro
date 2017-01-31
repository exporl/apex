BASEDIR = ../../../../../..
CLEBS += filterplugin buildplugin sndfile

include ($$BASEDIR/clebs.pri)

TARGET = sinkfilter

SOURCES +=                                              \
        sinkfilter.cpp					\
