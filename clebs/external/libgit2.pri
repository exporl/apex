clebsCheck(libgit2) {
    win32 {
        isEmpty(LIBGIT2ROOT):LIBGIT2ROOT = $$BASEDIR/../api/libgit2
        isEmpty(LIBGIT2BINDIR):LIBGIT2BINDIR = $${LIBGIT2ROOT}/bin
        isEmpty(LIBGIT2LIBDIR):LIBGIT2LIBDIR = $${LIBGIT2ROOT}/lib
        isEmpty(LIBGIT2LIB):LIBGIT2LIB = git2

        exists($${LIBGIT2ROOT}/include/git2.h):CLEBS_DEPENDENCIES *= libgit2
    }

    unix {
        system(pkg-config libgit2):CLEBS_DEPENDENCIES *= libgit2
    }
}

clebsDependency(libgit2) {
    win32 {
        INCLUDEPATH *= $${LIBGIT2ROOT}/include
        LIBS *= -L$${LIBGIT2LIBDIR} -l$${LIBGIT2LIB}

    }

    unix {
        PKGCONFIG *= libgit2
    }
}

clebsInstall(libgit2) {
    win32 {
        libgit2install.files = $${LIBGIT2BINDIR}/$${LIBGIT2LIB}.dll

        libgit2install.path = $$BINDIR
        INSTALLS *= libgit2install
    }
}
