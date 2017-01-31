CLEBS *= asciicast builddll

DEFINES *= PSIGNIFIT_MAKEDLL

!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

PFDIR = $$BASEDIR/contrib/psignifit
INCLUDEPATH *= $$PFDIR

SOURCES += $$files($$PFDIR/*.c)
HEADERS += $$files($$PFDIR/*.h)

SOURCES -= $$PFDIR/main.c
