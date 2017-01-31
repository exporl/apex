BASEDIR = ../../..
CLEBS *= buildplugin streamapp win32
TARGET = clarionplugin

include($${BASEDIR}/clebs.pri)

LIBS *= ole32.lib oleaut32.lib uuid.lib

INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/stimulus/clarion
DEPENDPATH *= $$BASEDIR/src/lib/apexmain/stimulus/clarion

SOURCES *= activexclarion.cpp

HEADERS *= activexclarion.h
