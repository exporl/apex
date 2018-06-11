CLEBS *= builddll common
TARGET = exporlcommongui
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

!android:contains(DEFINES, WITH_WEBENGINE) {
    QT *= webenginewidgets
} else:!android {
    QT *= webkitwidgets
}

android:QT *= qml quick webview quickwidgets
android:RESOURCES = commongui.qrc

FORMS *= listpicker.ui

DEFINES *= COMMON_MAKEDLL
