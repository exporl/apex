clebsCheck(fftw3f) {
    win32 {
	isEmpty(FFTW3FROOT):FFTW3FROOT = $$BASEDIR/../api/fftw3
	isEmpty(FFTW3FLIB):FFTW3FLIB = libfftw3f-3
	isEmpty(FFTW3FLIB_RELEASE):FFTW3FLIB_RELEASE = $${FFTW3FLIB}
	isEmpty(FFTW3FLIB_DEBUG):FFTW3FLIB_DEBUG = $${FFTW3FLIB}
	isEmpty(FFTW3FLIBDIR):FFTW3FLIBDIR = $${FFTW3FROOT}
	isEmpty(FFTW3FLIBDIR_RELEASE):FFTW3FLIBDIR_RELEASE = $${FFTW3FLIBDIR}
	isEmpty(FFTW3FLIBDIR_DEBUG):FFTW3FLIBDIR_DEBUG = $${FFTW3FLIBDIR}
	isEmpty(FFTW3FINCLUDEDIR):FFTW3FINCLUDEDIR = $${FFTW3FROOT}

	exists($${FFTW3FROOT}):CLEBS_DEPENDENCIES *= fftw3f
    }

    unix {
	system(pkg-config fftw3f):CLEBS_DEPENDENCIES *= fftw3f
    }
}

clebsDependency(fftw3f) {
    win32 {
        INCLUDEPATH *= $${FFTW3FROOT}
	CONFIG(debug, debug|release) {
	    LIBS *= -L$${FFTW3FLIBDIR_DEBUG} -l$${FFTW3FLIB_DEBUG}
	} else {
	    LIBS *= -L$${FFTW3FLIBDIR_RELEASE} -l$${FFTW3FLIB_RELEASE}
	}
    }

    unix {
	PKGCONFIG *= fftw3f
    }
}

clebsInstall(fftw3f) {
    win32 {
	CONFIG(debug, debug|release) {
	    fftw3finstall.files = $${FFTW3FLIBDIR_DEBUG}/$${FFTW3FLIB_DEBUG}.dll
	} else {
	    fftw3finstall.files = $${FFTW3FLIBDIR_RELEASE}/$${FFTW3FLIB_RELEASE}.dll
	}
	fftw3finstall.path = $$BINDIR
	INSTALLS *= fftw3finstall
    }
}
