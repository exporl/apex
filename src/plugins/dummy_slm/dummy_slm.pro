BASEDIR = ../../..
CLEBS *= buildplugin
TARGET = dummy_slm

include ($$BASEDIR/clebs.pri)

# WTF? TODO FIXME path???
INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/


SOURCES *=	dummy_slm.cpp	

# WTF? TODO FIXME path???
HEADERS +=	$$BASEDIR/src/lib/apexmain/calibration/SoundLevelMeter.h \

