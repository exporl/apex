BASEDIR = ../../../../..
CLEBS += syllib filterplugin sndfile

include ($$BASEDIR/clebs.pri)

TARGET = syltester
CONFIG += qtestlib

SOURCES +=						\
	arfilter.cpp					\
	filesink.cpp					\
	firfilter.cpp					\
	hrtfmwf.cpp					\
	hrtfmwfadapt.cpp				\
	tester.cpp					\
	threadedfirfilter.cpp				\
	vad.cpp						\

HEADERS +=						\
        tester.h					\
