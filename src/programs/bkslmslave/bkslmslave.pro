!android {
CLEBS *= csc
!isEmpty(_PRO_FILE_):include(../../../clebs/clebs.pri)

QMAKE_LFLAGS = /r:Google.ProtocolBuffers.dll
CS_SOURCES = bkslmslave.cs slaveclient.cs protoslave.cs

unix {
    QMAKE_LFLAGS *= /r:Mono.Posix.dll
    CS_SOURCES *= dummysoundlevelmeter.cs
} else {
    QMAKE_LFLAGS *= /r:BasicEnvRemoteAPI.dll
    CS_SOURCES *= bksoundlevelmeter.cs
}

csdlls.path = $$BINDIR
csdlls.files = *.dll
INSTALLS *= csdlls
}
