CLEBS *= apextools win32 asciicast buildplugin

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= gui widgets axcontainer

CONFIG += console

# TODO
HEADERS *= ../../../src/lib/apexmain/gui/flashwidget.h
SOURCES *= ../../../src/lib/apexmain/gui/flashwidget.cpp
