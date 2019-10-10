CLEBS *= builddll common
TARGET = exporlcommongui
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

!android:QT *= webenginewidgets
android:QT *= qml quick webview quickwidgets
android:RESOURCES = commongui.qrc

FORMS *= listpicker.ui

DEFINES *= COMMON_MAKEDLL
