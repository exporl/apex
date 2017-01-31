BASEDIR =../../..
CLEBS += filterplugin buildplugin 

include ($$BASEDIR/clebs.pri)

TARGET = delayfilter

SOURCES +=                                              \
        delayfilter.cpp                             \
