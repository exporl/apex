CONFIG += qt

# WTF? TODO FIXME no clebs???

# WTF? TODO FIXME path???
CLEBSPATH = ../../../src/

# WTF? TODO FIXME path???
INCLUDEPATH += $$CLEBSPATH

SOURCES += testarclayout.cpp

# WTF? TODO FIXME path???
HEADERS +=$${CLEBSPATH}/gui/arclayout.h
SOURCES+= $${CLEBSPATH}/gui/arclayout.cpp

