BASEDIR = ../../../
CLEBS += buildplugin
TARGET = l34fileplugin

include($${BASEDIR}/clebs.pri)

INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/stimulus/l34
DEPENDPATH *= $$BASEDIR/src/lib/apexmain/stimulus/l34

SOURCES *= l34file.cpp
