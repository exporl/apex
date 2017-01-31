BASEDIR = ../../..
CLEBS += filterplugin buildplugin apextools

include ($$BASEDIR/clebs.pri)

TARGET = iirfilter

SOURCES +=                                              \
        iirfilter.cpp					\
