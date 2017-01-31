BASEDIR = ../../..
SPINDIR = $$BASEDIR/src/plugins/spin
CLEBS += apextools

INCLUDEPATH += $$BASEDIR/src/plugins/spin/ $$BASEDIR/src/lib/apexmain/
include($$BASEDIR/clebs.pri)

QT += gui script

DEFINES += NOSCRIPTEXPAND

SOURCES += \
	spintest.cpp \
#	$$SPINDIR/gui/spindialog.cpp \
#    $$SPINDIR/data/spinconfig.cpp \
#    $$SPINDIR/xml/spinconfigfileparser.cpp \

HEADERS += \
#	$$SPINDIR/gui/spindialog.h \
#    $$SPINDIR/data/spinconfig.h \
#    $$SPINDIR/xml/spinconfigfileparser.h \

FORMS += \
#    $$SPINDIR/gui/spinmaindlg.ui \

#LIBS *= -lspin -L$$BASEDIR/bin/debug




















