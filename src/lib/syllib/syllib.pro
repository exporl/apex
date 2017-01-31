BASEDIR = ../../..
CLEBS += fftw3 builddll pch

include ($$BASEDIR/clebs.pri)

TARGET = syllib
DEFINES += SYL_MAKEDLL 

SOURCES +=						\
#        adaptivewiener.cpp				\
        arfilter.cpp					\
        firfilter.cpp					\
        threadedfirfilter.cpp				\
        vad.cpp						\

HEADERS +=						\
#        adaptivewiener.h				\
        arfilter.h					\
        exception.h					\
        eventthread.h					\
        firfilter.h					\
        global.h					\
        threadedfirfilter.h				\
        vad.h						\
