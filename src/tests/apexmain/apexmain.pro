CLEBS *= apexmain common asciicast
TARGET = apexmaintest

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= testlib gui widgets webkitwidgets network xml

android {
    QT -= webkitwidgets
    QT *= webview
}
