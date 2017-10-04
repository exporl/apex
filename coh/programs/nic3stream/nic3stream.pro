CLEBS *= nic3 python
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

PYTHON_SOURCES = nic3stream.py

for(source, PYTHON_SOURCES):pythoninstall.files += $$DESTDIR/$$source
pythoninstall.path = $$BINDIR
pythoninstall.CONFIG = no_check_exist
INSTALLS += pythoninstall
