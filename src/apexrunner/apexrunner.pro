BASEDIR = ../..
TARGET = apex
CLEBS += apexmain

include ($$BASEDIR/clebs.pri)

win32:RC_FILE = $$BASEDIR/data/data/icon.rc

SOURCES = apex.cpp
