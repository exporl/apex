BASEDIR = ../../../../../..
CLEBS += filterplugin buildplugin syllib

include ($$BASEDIR/clebs.pri)

TARGET = beamformerfilter

SOURCES +=                                              \
        beamformerfilter.cpp				\
