debug:LIBS *= -lstreamappd
release:LIBS *= -lstreamapp

INCLUDEPATH *= $$BASEDIR/src/lib/streamapp $$BASEDIR/src/lib/streamapp/_fromv3
DEPENDPATH *= $$BASEDIR/src/lib/streamapp $$BASEDIR/src/lib/streamapp/_fromv3

unix:INCLUDEPATH *= $$BASEDIR/src/lib/streamapp/_archs/linux
unix:DEPENDPATH *= $$BASEDIR/src/lib/streamapp/_archs/linux
win32:INCLUDEPATH *= $$BASEDIR/src/lib/streamapp/_archs/win32
win32:DEPENDPATH *= $$BASEDIR/src/lib/streamapp/_archs/win32
