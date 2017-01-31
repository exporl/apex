BASEDIR = ../../..
CLEBS += filterplugin buildplugin apextools

include ($$BASEDIR/clebs.pri)

TARGET = scramblespectrumfilter

SOURCES +=                                              \
        scramblespectrumfilter.cpp					\
