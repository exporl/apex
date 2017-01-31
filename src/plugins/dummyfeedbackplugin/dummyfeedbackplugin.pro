BASEDIR = ../../..
CLEBS *= buildplugin
TARGET = dummyfeedbackplugin

include ($$BASEDIR/clebs.pri)

# WTF? TODO FIXME path???
INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/


SOURCES *=	dummyfeedbackplugin.cpp	

# WTF? TODO FIXME path???
HEADERS +=	$$BASEDIR/src/lib/apexmain/feedback/pluginfeedbackinterface.h \

