CLEBS *= builddll common
TARGET = exporlcommongui
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= qml quick webview quickwidgets
RESOURCES = commongui.qrc

FORMS *= listpicker.ui

DEFINES *= COMMON_MAKEDLL
