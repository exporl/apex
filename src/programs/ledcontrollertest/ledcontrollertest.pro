CLEBS *= apextools iowkit asciicast

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

CONFIG *= console

QT *= widgets

INCLUDEPATH *= ../../../src/plugins/ledfeedbackplugin
HEADERS *= ../../../src/plugins/ledfeedbackplugin/ledcontroller.h
SOURCES *= ../../../src/plugins/ledfeedbackplugin/ledcontroller.cpp

FORMS = $$files(*.ui)
