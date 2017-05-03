CLEBS *= +xml2 common subdirsources asciicast builddll

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT *= gui widgets xml xmlpatterns network script

DEFINES *= APEXTOOLS_MAKEDLL

win32:LIBS *= -lshell32 # ShellExecute
