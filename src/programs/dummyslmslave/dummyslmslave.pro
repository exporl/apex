CLEBS *= protobuf python
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

PYTHON_SOURCES = dummyslmslave.py
PYTHON_PROTOS = ../../../common/data/protoslave.proto

for(source, PYTHON_SOURCES):pythoninstall.files += $$DESTDIR/$$source
for(proto, PYTHON_PROTOS):pythoninstall.files += $$DESTDIR/$$replace(proto, ".*/([^/]*)\\.proto", "\\1")_pb2.py
pythoninstall.path = $$BINDIR
pythoninstall.CONFIG = no_check_exist
INSTALLS += pythoninstall
