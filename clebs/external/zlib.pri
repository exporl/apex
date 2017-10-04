clebsCheck(zlib) {
    win32 {
        isEmpty(ZLIBROOT):ZLIBROOT = $$BASEDIR/../api/zlib
        isEmpty(ZLIBINCLUDEDIR):ZLIBINCLUDEDIR = $${ZLIBROOT}/include
        isEmpty(ZLIBBINDIR):ZLIBBINDIR = $${ZLIBROOT}/bin
        isEmpty(ZLIBLIBDIR):ZLIBLIBDIR = $${ZLIBROOT}/lib
        isEmpty(ZLIBDLL):ZLIBDLL = zlib1
        isEmpty(ZLIBLIB):ZLIBLIB = zlib

        exists($${ZLIBINCLUDEDIR}):CLEBS_DEPENDENCIES *= zlib
    }

    unix {
        system(pkg-config zlib):CLEBS_DEPENDENCIES *= zlib
    }
}

clebsDependency(zlib) {
    win32 {
        LIBS *= -l$${ZLIBLIB} -L$${ZLIBLIBDIR}
        INCLUDEPATH *= $${ZLIBINCLUDEDIR}
    }
    unix {
        PKGCONFIG *= zlib
    }
}

clebsInstall(zlib) {
    win32 {
        zlibinstall.files = $${ZLIBBINDIR}/$${ZLIBDLL}.dll
        zlibinstall.path = $$BINDIR
        INSTALLS *= zlibinstall
    }
}
