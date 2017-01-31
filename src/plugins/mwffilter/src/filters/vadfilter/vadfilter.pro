BASEDIR = ../../../../../..
# Added fftw3 to avoid building this plugin if syllib can't be built
CLEBS += filterplugin buildplugin syllib fftw3

include ($$BASEDIR/clebs.pri)

TARGET = vadfilter

SOURCES +=                                              \
        vadfilter.cpp					\
