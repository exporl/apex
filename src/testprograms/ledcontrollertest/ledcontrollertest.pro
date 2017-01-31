TEMPLATE = app
BASEDIR = ../../..
CLEBS*=iowkit
TARGET=ledcontrollertest

include ($$BASEDIR/clebs.pri)

CONFIG += console
QT += gui

INCLUDEPATH += $$BASEDIR/src/plugins/ledfeedback/

HEADERS  = $$BASEDIR/src/plugins/ledfeedback/ledcontroller.h \
			ledcontrollertest.h

SOURCES	 = main.cpp \
    ledcontrollertest.cpp \
    $$BASEDIR/src/plugins/ledfeedback/ledcontroller.cpp \

FORMS = ledcontrollertest.ui
