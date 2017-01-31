BASEDIR =../../../..
CLEBS *= win32
include ($$BASEDIR/clebs.pri)

QT *= gui
CONFIG += console qaxcontainer

INCLUDEPATH += $$BASEDIR/src/plugins/slm_2250

HEADERS = \
    $$BASEDIR/src/plugins/slm_2250/basicenvremoteapi.h \
    $$BASEDIR/src/plugins/slm_2250/axbase.h \
    $$BASEDIR/src/plugins/slm_2250/axobject.h \
    $$BASEDIR/src/plugins/slm_2250/axtypes.h \

SOURCES = \
	$$BASEDIR/src/plugins/slm_2250/basicenvremoteapi.cpp \
    $$BASEDIR/src/plugins/slm_2250/axbase.cpp \
    $$BASEDIR/src/plugins/slm_2250/axobject.cpp \
    2250test.cpp

