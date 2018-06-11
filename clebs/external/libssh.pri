clebsCheck(libssh) {
    win32 {
        isEmpty(LIBSSHROOT):LIBSSHROOT = $$BASEDIR/../api/libssh
        isEmpty(LIBSSHBINDIR):LIBSSHBINDIR = $${LIBSSHROOT}/bin
        isEmpty(LIBSSHLIBDIR):LIBSSHLIBDIR = $${LIBSSHROOT}/lib
        isEmpty(LIBSSHLIB):LIBSSHLIB = ssh

        exists($${LIBSSHROOT}/include/libssh/libssh.h):CLEBS_DEPENDENCIES *= libssh
    }

    unix {
        system(pkg-config libssh):CLEBS_DEPENDENCIES *= libssh
    }
}

clebsDependency(libssh) {
    win32 {
        INCLUDEPATH *= $${LIBSSHROOT}/include
        LIBS *= -L$${LIBSSHLIBDIR} -l$${LIBSSHLIB}

    }

    unix {
        PKGCONFIG *= libssh
    }
}

clebsInstall(libssh) {
    win32 {
        libsshinstall.files = $${LIBSSHBINDIR}/$${LIBSSHLIB}.dll

        libsshinstall.path = $$BINDIR
        INSTALLS *= libsshinstall
    }
}
