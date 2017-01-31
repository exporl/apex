BASEDIR = ../../../../..
# Added fftw3 to avoid building this plugin if syllib can't be built
CLEBS += syllib filterplugin fftw3

include ($$BASEDIR/clebs.pri)

TARGET = profile

SOURCES +=						\
	profile.cpp					\
