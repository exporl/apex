#unix:CONFIG += hide_symbols
#CONFIG += dll
TEMPLATE = app

TARGET = democontroller
CONFIG += plugin
CONFIG += debug

QT -= gui

# WTF? TODO FIXME path???
INCLUDEPATH +=						\
	../../src/device

SOURCES +=						\
	democontroller.cpp				\
	tester.cpp

HEADERS +=						\
	democontroller.h 				\
