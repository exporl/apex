CLEBS *= syllib sndfile fftw3
TARGET = syllibtest
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += testlib
