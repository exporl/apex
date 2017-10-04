CLEBS *= common builddll +fftw3f +sndfile
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

DEFINES += BERTHA_MAKEDLL

QT *= xml xmlpatterns network

win32:LIBS *= -lshell32
