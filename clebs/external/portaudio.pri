clebsCheck(portaudio) {
    win32 {
        isEmpty(PORTAUDIOROOT):PORTAUDIOROOT = $$BASEDIR/../api/portaudio
        isEmpty(PORTAUDIOLIB):PORTAUDIOLIB = portaudio_x86
        isEmpty(PORTAUDIOLIB_RELEASE):PORTAUDIOLIB_RELEASE = $${PORTAUDIOLIB}
        isEmpty(PORTAUDIOLIB_DEBUG):PORTAUDIOLIB_DEBUG = $${PORTAUDIOLIB}
        isEmpty(PORTAUDIOLIBDIR):PORTAUDIOLIBDIR = $${PORTAUDIOROOT}/lib
        isEmpty(PORTAUDIOLIBDIR_RELEASE):PORTAUDIOLIBDIR_RELEASE = $${PORTAUDIOLIBDIR}/release
        isEmpty(PORTAUDIOLIBDIR_DEBUG):PORTAUDIOLIBDIR_DEBUG = $${PORTAUDIOLIBDIR}/debug
        isEmpty(PORTAUDIODLL):PORTAUDIODLL = $${PORTAUDIOLIB}
        isEmpty(PORTAUDIODLL_RELEASE):PORTAUDIODLL_RELEASE = $${PORTAUDIODLL}.dll
        isEmpty(PORTAUDIODLL_DEBUG):PORTAUDIODLL_DEBUG = $${PORTAUDIODLL}.dll
        isEmpty(PORTAUDIODLLDIR):PORTAUDIODLLDIR = $${PORTAUDIOLIBDIR}
        isEmpty(PORTAUDIODLLDIR_RELEASE):PORTAUDIODLLDIR_RELEASE = $${PORTAUDIODLLDIR}/release
        isEmpty(PORTAUDIODLLDIR_DEBUG):PORTAUDIODLLDIR_DEBUG = $${PORTAUDIODLLDIR}/debug
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
            portaudioinstall.files = $${PORTAUDIODLLDIR_DEBUG}/$${PORTAUDIODLL_DEBUG}
        } else {
            portaudioinstall.files = $${PORTAUDIODLLDIR_RELEASE}/$${PORTAUDIODLL_RELEASE}
        }
        portaudioinstall.path = $$BINDIR
        INSTALLS *= portaudioinstall
    }
}
