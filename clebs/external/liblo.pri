clebsCheck(liblo) {
    win32 {
        isEmpty(LIBLOROOT):LIBLOROOT = $$BASEDIR/../api/liblo
        isEmpty(LIBLOLIB):LIBLOLIB = lo
        isEmpty(LIBLOLIBDIR_RELEASE):LIBLOLIBDIR_RELEASE = $${LIBLOROOT}/release/lib
        isEmpty(LIBLOLIBDIR_DEBUG):LIBLOLIBDIR_DEBUG = $${LIBLOROOT}/debug/lib
        isEmpty(LIBLOINCLUDEDIR):LIBLOINCLUDEDIR = $${LIBLOROOT}/include

        exists($${LIBLOROOT}):CLEBS_DEPENDENCIES *= liblo
    }

    unix {
        system(pkg-config liblo):CLEBS_DEPENDENCIES *= liblo
    }
}

clebsDependency(liblo) {
    win32 {
        INCLUDEPATH *= $${LIBLOINCLUDEDIR}
        LIBS *= -lws2_32 -lIphlpapi
        CONFIG(debug, debug|release) {
            LIBS *= -L$${LIBLOLIBDIR_DEBUG} -l$${LIBLOLIB}
        } else {
            LIBS *= -L$${LIBLOLIBDIR_RELEASE} -l$${LIBLOLIB}
        }
    }

    unix {
        PKGCONFIG *= liblo
    }
}

clebsInstall(liblo) {
    win32 {
        CONFIG(debug, debug|release) {
            libloinstall.files = $${LIBLOLIBDIR_DEBUG}/$${LIBLOLIB}.dll
        } else {
            libloinstall.files = $${LIBLOLIBDIR_RELEASE}/$${LIBLOLIB}.dll
        }

        libloinstall.path = $$BINDIR

        INSTALLS += libloinstall

    }
}
