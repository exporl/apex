CLEBS *= apexcaladmin apextools asciicast

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= core gui
greaterThan(QT_MAJOR_VERSION, 4):QT *= widgets

win32 {
    CONFIG += embed_manifest_exe
    QMAKE_LFLAGS_WINDOWS += $$quote( /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\" )
}
