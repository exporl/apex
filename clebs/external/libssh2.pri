clebsCheck(libssh2) {
    win32 {
        isEmpty(LIBSSH2ROOT):LIBSSH2ROOT = $$BASEDIR/../api/libssh2
        isEmpty(LIBSSH2BINDIR):LIBSSH2BINDIR = $${LIBSSH2ROOT}/bin
        isEmpty(LIBSSH2LIBDIR):LIBSSH2LIBDIR = $${LIBSSH2ROOT}/lib
        isEmpty(LIBSSH2LIB):LIBSSH2LIB = libssh2

        exists($${LIBSSH2ROOT}/include/libssh2.h):CLEBS_DEPENDENCIES *= libssh2
    }

    unix {
        system(pkg-config libssh2):CLEBS_DEPENDENCIES *= libssh2
    }
}

clebsDependency(libssh2) {
    win32 {
        INCLUDEPATH *= $${LIBSSH2ROOT}/include
        LIBS *= -L$${LIBSSH2LIBDIR} -l$${LIBSSH2LIB}

    }

    unix {
        PKGCONFIG *= libssh2
    }
}

clebsInstall(libssh2) {
    win32 {
        libssh2install.files = $${LIBSSH2BINDIR}/$${LIBSSH2LIB}.dll

        libssh2install.path = $$BINDIR
        INSTALLS *= libssh2install
    }
}
