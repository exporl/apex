BASEDIR = ../../..
CLEBS += buildplugin nic
TARGET = l34plugin

include($${BASEDIR}/clebs.pri)

INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/stimulus/l34
DEPENDPATH *= $$BASEDIR/src/lib/apexmain/stimulus/l34

SOURCES *= apexnresocket.cpp

HEADERS *= apexnresocket.h
