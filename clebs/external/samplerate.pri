clebsCheck(samplerate) {
    win32 {
        isEmpty(SAMPLERATEROOT):SAMPLERATEROOT = $$BASEDIR/../api/samplerate
        isEmpty(SAMPLERATELIB):SAMPLERATELIB = libsamplerate-0
        isEmpty(SAMPLERATELIB_RELEASE):SAMPLERATELIB_RELEASE = $${SAMPLERATELIB}
        isEmpty(SAMPLERATELIB_DEBUG):SAMPLERATELIB_DEBUG = $${SAMPLERATELIB}
        isEmpty(SAMPLERATELIBDIR):SAMPLERATELIBDIR = $${SAMPLERATEROOT}
        isEmpty(SAMPLERATELIBDIR_RELEASE):SAMPLERATELIBDIR_RELEASE = $${SAMPLERATELIBDIR}/release
        isEmpty(SAMPLERATELIBDIR_DEBUG):SAMPLERATELIBDIR_DEBUG = $${SAMPLERATELIBDIR}/debug
        isEmpty(SAMPLERATEINCLUDEDIR):SAMPLERATEINCLUDEDIR = $${SAMPLERATEROOT}

        exists($${SAMPLERATEROOT}):CLEBS_DEPENDENCIES *= samplerate
    }

    unix {
        system(pkg-config samplerate):CLEBS_DEPENDENCIES *= samplerate
    }
}

clebsDependency(samplerate) {
    win32 {
        INCLUDEPATH *= $${SAMPLERATEINCLUDEDIR}
        CONFIG(debug, debug|release) {
            LIBS *= -L$${SAMPLERATELIBDIR_DEBUG} -l$${SAMPLERATELIB_DEBUG}
        } else {
            LIBS *= -L$${SAMPLERATELIBDIR_RELEASE} -l$${SAMPLERATELIB_RELEASE}
        }
    }

    unix {
        PKGCONFIG *= samplerate
    }
}

clebsInstall(samplerate) {
    win32 {
        CONFIG(debug, debug|release) {
            samplerateinstall.files = $${SAMPLERATELIBDIR_DEBUG}/$${SAMPLERATELIB_DEBUG}.dll
        } else {
            samplerateinstall.files = $${SAMPLERATELIBDIR_RELEASE}/$${SAMPLERATELIB_RELEASE}.dll
        }
        samplerateinstall.path = $$BINDIR
        INSTALLS *= samplerateinstall
    }
}

