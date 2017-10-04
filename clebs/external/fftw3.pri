clebsCheck(fftw3) {
    win32 {
        isEmpty(FFTW3ROOT):FFTW3ROOT = $$BASEDIR/../api/fftw3
        isEmpty(FFTW3LIB):FFTW3LIB = libfftw3-3
        isEmpty(FFTW3LIB_RELEASE):FFTW3LIB_RELEASE = $${FFTW3LIB}
        isEmpty(FFTW3LIB_DEBUG):FFTW3LIB_DEBUG = $${FFTW3LIB}
        isEmpty(FFTW3LIBDIR):FFTW3LIBDIR = $${FFTW3ROOT}
        isEmpty(FFTW3LIBDIR_RELEASE):FFTW3LIBDIR_RELEASE = $${FFTW3LIBDIR}
        isEmpty(FFTW3LIBDIR_DEBUG):FFTW3LIBDIR_DEBUG = $${FFTW3LIBDIR}
        isEmpty(FFTW3INCLUDEDIR):FFTW3INCLUDEDIR = $${FFTW3ROOT}

        exists($${FFTW3ROOT}):CLEBS_DEPENDENCIES *= fftw3
    }

    unix {
        system(pkg-config fftw3):CLEBS_DEPENDENCIES *= fftw3
    }
}

clebsDependency(fftw3) {
    win32 {
        INCLUDEPATH *= $${FFTW3INCLUDEDIR}
        CONFIG(debug, debug|release) {
            LIBS *= -L$${FFTW3LIBDIR_DEBUG} -l$${FFTW3LIB_DEBUG}
        } else {
            LIBS *= -L$${FFTW3LIBDIR_RELEASE} -l$${FFTW3LIB_RELEASE}
        }
    }

    unix {
        PKGCONFIG *= fftw3
    }
}

clebsInstall(fftw3) {
    win32 {
        CONFIG(debug, debug|release) {
            fftw3install.files = $${FFTW3LIBDIR_DEBUG}/$${FFTW3LIB_DEBUG}.dll
        } else {
            fftw3install.files = $${FFTW3LIBDIR_RELEASE}/$${FFTW3LIB_RELEASE}.dll
        }
        fftw3install.path = $$BINDIR
        INSTALLS *= fftw3install
    }
}
