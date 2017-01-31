BASEDIR = ../../..
CLEBS *= buildplugin iowkit feedbackplugin
TARGET = ledfeedback

include ($$BASEDIR/clebs.pri)

SOURCES *=	ledfeedback.cpp \
                ledcontroller.cpp \

HEADERS +=	ledcontroller.h \
