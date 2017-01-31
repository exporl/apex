BASEDIR = ../../..
CLEBS += runnerplugin buildplugin apexmain apextools apexwriters apexdata
TARGET = spin

DEFINES += SPIN_MAKEDLL

include ($$BASEDIR/clebs.pri)

INCLUDEPATH += .


QT += gui

SOURCES += \
    spin.cpp \
    spinexperimentcreator.cpp \
    spincalibrator.cpp \
    gui/decibellineedit.cpp \
    gui/decibellabel.cpp \
    gui/noisestartpointdialog.cpp \
    gui/spindialog.cpp \
    gui/startlevelwidget.cpp \
    gui/freefieldwidget.cpp \
    gui/snrwidget.cpp \
    data/spinconfig.cpp \
    data/spinusersettings.cpp \
    data/spinusersettingsdatabase.cpp \
    xml/spinconfigfileparser.cpp \

HEADERS += \
    spinexperimentconstants.h \
    spinexperimentcreator.h \
    spincalibrator.h \
    gui/decibellineedit.h \
    gui/decibellabel.h \
    gui/noisestartpointdialog.h \
    gui/spindialog.h \
    gui/startlevelwidget.h \
    gui/freefieldwidget.h \
    gui/snrwidget.h \
    data/spinconfig.h \
    data/spindataenums.h \
    data/spinusersettings.h \
    data/spinusersettingsdatabase.h \
    data/spindefaultsettings.h \
    xml/spinconfigfileparser.h \
	
FORMS += \
    gui/spinmaindlg.ui \
    gui/startlevelwidget.ui \
    gui/noisestartpointdlg.ui \
    gui/snrwidget.ui \

RESOURCES = gui/resources.qrc
