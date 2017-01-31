BASEDIR = ../../../../../..
CLEBS += filterplugin buildplugin syllib

include ($$BASEDIR/clebs.pri)

TARGET = adaptivewienerfilter

SOURCES +=                                              \
        adaptivewienerfilter.cpp			\
