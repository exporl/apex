BASEDIR = ../../..
CLEBS += lib libwriters libtools libdata xerces

include($$BASEDIR/clebs.pri)

QT += gui script testlib
CONFIG += qtestlib

DEFINES += DEBUG WRITERTEST NOSCRIPTEXPAND

SOURCES += \
	unittest.cpp \
#	testwriters.cpp \
#	writertester.cpp \
	tools.cpp \
#	datablockstester.cpp \
#	domtreeerrorreporter.cpp \
#	devicestester.cpp \
#	procedurestester.cpp \
#	filterstester.cpp \
#    screenstester.cpp \
#    stimulitester.cpp \
#	experimenttester.cpp \


HEADERS += \
	unittest.h \
	tools.h \
#	writertester.h \
#	datablockstester.h \
#	DomTreeErrorReporter.h \
#	devicestester.h \
#	procedurestester.h \
#	filterstester.h \
#    screenstester.h \
#    stimulitester.h \
#	experimenttester.h \



















