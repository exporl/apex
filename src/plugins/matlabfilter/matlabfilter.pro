BASEDIR =../../..
CLEBS += filterplugin buildplugin matlab

include ($$BASEDIR/clebs.pri)

TARGET = matlabfilter

SOURCES +=                                              \
        matlabfilter.cpp                             \
