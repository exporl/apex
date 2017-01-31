BASEDIR = ../../../../../..
CLEBS += filterplugin buildplugin syllib

include ($$BASEDIR/clebs.pri)

TARGET = wienerfilter

SOURCES +=                                              \
        wienerfilter.cpp				\
