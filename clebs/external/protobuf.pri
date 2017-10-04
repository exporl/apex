clebsCheck(protobuf) {
    win32 {
        isEmpty(PROTOBUFROOT):PROTOBUFROOT = $$BASEDIR/../api/protobuf
        isEmpty(PROTOBUFLIB):PROTOBUFLIB = libprotobuf-lite
        isEmpty(PROTOBUFLIB_RELEASE):PROTOBUFLIB_RELEASE = $${PROTOBUFLIB}
        isEmpty(PROTOBUFLIB_DEBUG):PROTOBUFLIB_DEBUG = $${PROTOBUFLIB}
        isEmpty(PROTOBUFLIB_VERSION):PROTOBUFLIB_VERSION = -8
        isEmpty(PROTOBUFLIBDIR):PROTOBUFLIBDIR = $${PROTOBUFROOT}
        isEmpty(PROTOBUFLIBDIR_RELEASE):PROTOBUFLIBDIR_RELEASE = $${PROTOBUFLIBDIR}/release
        isEmpty(PROTOBUFLIBDIR_DEBUG):PROTOBUFLIBDIR_DEBUG = $${PROTOBUFLIBDIR}/debug
        isEmpty(PROTOBUFINCLUDEDIR):PROTOBUFINCLUDEDIR = $${PROTOBUFROOT}/include
        isEmpty(PROTOBUFPROTOC):PROTOBUFPROTOC = $${PROTOBUFROOT}/protoc.exe
        isEmpty(PROTOBUFPYTHONDIR):PROTOBUFPYTHONDIR = $${PROTOBUFROOT}/python-lib

        exists($${PROTOBUFROOT}):CLEBS_DEPENDENCIES *= protobuf
    }

    unix {
        PROTOBUFPROTOC = protoc
        system(pkg-config protobuf-lite):system(which $${PROTOBUFPROTOC} 2>&1 > /dev/null):CLEBS_DEPENDENCIES *= protobuf
    }
}

clebsDependency(protobuf) {
    win32 {
        INCLUDEPATH *= $${PROTOBUFINCLUDEDIR}
        CONFIG(debug, debug|release) {
            LIBS *= -L$${PROTOBUFLIBDIR_DEBUG} -l$${PROTOBUFLIB_DEBUG}
        } else {
            LIBS *= -L$${PROTOBUFLIBDIR_RELEASE} -l$${PROTOBUFLIB_RELEASE}
        }
    }

    unix {
        PKGCONFIG *= protobuf-lite
    }

    PROTOPATHS = --proto_path ../../data --proto_path ../../../common/data
    INCLUDEPATH *= $$OBJECTS_DIR

    protobuf_decl.input = PROTOS
    protobuf_decl.output = $$OBJECTS_DIR/${QMAKE_FILE_BASE}.pb.h
    protobuf_decl.commands = @echo compiling protobuf $< && $$PROTOBUFPROTOC --cpp_out=$$OBJECTS_DIR $$PROTOPATHS ${QMAKE_FILE_NAME}
    protobuf_decl.variable_out = GENERATED_FILES

    protobuf_impl.input = PROTOS
    protobuf_impl.output = $$OBJECTS_DIR/${QMAKE_FILE_BASE}.pb.cc
    protobuf_impl.depends = $$OBJECTS_DIR/${QMAKE_FILE_BASE}.pb.h
    protobuf_impl.commands = $$escape_expand(\\n)
    protobuf_impl.variable_out = GENERATED_SOURCES

    protobuf_python.input = PYTHON_PROTOS
    protobuf_python.output = $$DESTDIR/${QMAKE_FILE_BASE}_pb2.py
    protobuf_python.commands = @echo compiling protobuf ${QMAKE_FILE_NAME} && $$PROTOBUFPROTOC --python_out=$$DESTDIR $$PROTOPATHS ${QMAKE_FILE_NAME}
    protobuf_python.variable_out = GENERATED_FILES
    protobuf_python.CONFIG += target_predeps

    QMAKE_EXTRA_COMPILERS += protobuf_decl protobuf_impl protobuf_python
}

clebsInstall(protobuf) {
    win32 {
        CONFIG(debug, debug|release) {
            protobufinstall.files = $${PROTOBUFLIBDIR_DEBUG}/$${PROTOBUFLIB_DEBUG}$${PROTOBUFLIB_VERSION}.dll
        } else {
            protobufinstall.files = $${PROTOBUFLIBDIR_RELEASE}/$${PROTOBUFLIB_RELEASE}$${PROTOBUFLIB_VERSION}.dll
        }
        protobufinstall.path = $$BINDIR

        pbpygoogleinstall.files = $$PROTOBUFPYTHONDIR/google/*.py
        pbpygoogleinstall.path = $$BINDIR/google

        pbpyprotobufinstall.files = $$PROTOBUFPYTHONDIR/google/protobuf/*.py
        pbpyprotobufinstall.path = $$BINDIR/google/protobuf

        pbpycompilerinstall.files = $$PROTOBUFPYTHONDIR/google/protobuf/compiler/*.py
        pbpycompilerinstall.path = $$BINDIR/google/protobuf/compiler

        pbpyinternalinstall.files = $$PROTOBUFPYTHONDIR/google/protobuf/internal/*.py
        pbpyinternalinstall.path = $$BINDIR/google/protobuf/internal

        INSTALLS *= protobufinstall pbpygoogleinstall pbpyprotobufinstall pbpycompilerinstall pbpyinternalinstall
    }
}
