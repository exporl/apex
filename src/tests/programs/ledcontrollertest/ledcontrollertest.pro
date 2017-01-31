TEMPLATE = app
BASEDIR = ../../../..
CLEBS*=iowkit
TARGET=ledcontrollertest

include ($$BASEDIR/clebs.pri)

CONFIG += console
QT += gui

# WTF? TODO FIXME path???
INCLUDEPATH += ../../../plugins/ledfeedback/


# WTF? TODO FIXME path???
HEADERS  = ../../../plugins/ledfeedback/ledcontroller.h \
			ledcontrollertest.h

# WTF? TODO FIXME path???
SOURCES	 = main.cpp \
    ledcontrollertest.cpp \
    ../../../plugins/ledfeedback/ledcontroller.cpp \

FORMS = ledcontrollertest.ui
