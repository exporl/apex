clebsCheck(portaudio) {
    win32 {
	isEmpty(PORTAUDIOROOT):PORTAUDIOROOT = $$BASEDIR/../api/portaudio
	isEmpty(PORTAUDIOLIB):PORTAUDIOLIB = portaudio_x86
	isEmpty(PORTAUDIOLIB_RELEASE):PORTAUDIOLIB_RELEASE = $${PORTAUDIOLIB}
	isEmpty(PORTAUDIOLIB_DEBUG):PORTAUDIOLIB_DEBUG = $${PORTAUDIOLIB}
	isEmpty(PORTAUDIOLIBDIR):PORTAUDIOLIBDIR = $${PORTAUDIOROOT}/lib
	isEmpty(PORTAUDIOLIBDIR_RELEASE):PORTAUDIOLIBDIR_RELEASE = $${PORTAUDIOLIBDIR}/release
	isEmpty(PORTAUDIOLIBDIR_DEBUG):PORTAUDIOLIBDIR_DEBUG = $${PORTAUDIOLIBDIR}/debug
	isEmpty(PORTAUDIOINCLUDEDIR):PORTAUDIOINCLUDEDIR = $${PORTAUDIOROOT}/include

	exists($${PORTAUDIOINCLUDEDIR}):CLEBS_DEPENDENCIES *= portaudio
    }

    unix {
	system(pkg-config portaudio-2.0):CLEBS_DEPENDENCIES *= portaudio
    }
}

clebsDependency(portaudio) {
    win32 {
	INCLUDEPATH *= $${PORTAUDIOINCLUDEDIR}
	CONFIG(debug, debug|release) {
	    LIBS *= -L$${PORTAUDIOLIBDIR_DEBUG} -l$${PORTAUDIOLIB_DEBUG}
	} else {
	    LIBS *= -L$${PORTAUDIOLIBDIR_RELEASE} -l$${PORTAUDIOLIB_RELEASE}
	}
    }

    unix {
	PKGCONFIG *= portaudio-2.0
    }
}

clebsInstall(portaudio) {
    win32 {
	CONFIG(debug, debug|release) {
	    portaudioinstall.files = $${PORTAUDIOLIBDIR_DEBUG}/$${PORTAUDIOLIB_DEBUG}.dll
	} else {
	    portaudioinstall.files = $${PORTAUDIOLIBDIR_RELEASE}/$${PORTAUDIOLIB_RELEASE}.dll
	}
	portaudioinstall.path = $$BINDIR
	INSTALLS *= portaudioinstall
    }
}
