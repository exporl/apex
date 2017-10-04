CLEBS *= builddll
TARGET = exporlcommongui
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= widgets network

!android {
 QT *=  webkitwidgets
}

android:QT *= qml quick webview quickwidgets
android:RESOURCES = commongui.qrc

DEFINES *= COMMON_MAKEDLL
