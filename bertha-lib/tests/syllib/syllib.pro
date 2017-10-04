CLEBS *= syllib common sndfile fftw3f
TARGET = syllibtest
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QT += testlib
