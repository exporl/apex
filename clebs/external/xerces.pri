clebsCheck(xerces) {
    win32 {
        isEmpty(XERCESCROOT):XERCESCROOT = $$BASEDIR/../api/xerces
        isEmpty(XERCESCLIB):XERCESCLIB = xerces-c_3
        isEmpty(XERCESCLIB_RELEASE):XERCESCLIB_RELEASE = $${XERCESCLIB}
        isEmpty(XERCESCLIB_DEBUG):XERCESCLIB_DEBUG = $${XERCESCLIB}d
        isEmpty(XERCESCLIBDIR):XERCESCLIBDIR = $${XERCESCROOT}/build/win32/vc10
        isEmpty(XERCESCLIBDIR_RELEASE):XERCESCLIBDIR_RELEASE = $${XERCESCLIBDIR}/release
        isEmpty(XERCESCLIBDIR_DEBUG):XERCESCLIBDIR_DEBUG = $${XERCESCLIBDIR}/debug
        isEmpty(XERCESCDLL):XERCESCDLL = xerces-c_3_1
        isEmpty(XERCESCDLL_RELEASE):XERCESCDLL_RELEASE = $${XERCESCDLL}.dll
        isEmpty(XERCESCDLL_DEBUG):XERCESCDLL_DEBUG = $${XERCESCDLL}d.dll
        isEmpty(XERCESCDLLDIR):XERCESCDLLDIR = $${XERCESCLIBDIR}
        isEmpty(XERCESCDLLDIR_RELEASE):XERCESCDLLDIR_RELEASE = $${XERCESCDLLDIR}/release
        isEmpty(XERCESCDLLDIR_DEBUG):XERCESCDLLDIR_DEBUG = $${XERCESCDLLDIR}/debug
        isEmpty(XERCESCINCLUDEDIR):XERCESCINCLUDEDIR = $${XERCESCROOT}/src

        exists ($${XERCESCROOT}):CLEBS_DEPENDENCIES *= xerces
    }

    unix {
        system(pkg-config xerces-c):CLEBS_DEPENDENCIES *= xerces
    }
}

clebsDependency(xerces) {
    win32 {
        INCLUDEPATH *= $${XERCESCINCLUDEDIR}
        CONFIG(debug,debug|release) {
            LIBS *= -L$${XERCESCLIBDIR_DEBUG} -l$${XERCESCLIB_DEBUG}
        } else {
            LIBS *= -L$${XERCESCLIBDIR_RELEASE} -l$${XERCESCLIB_RELEASE}
        }
    }

    unix {
        PKGCONFIG *= xerces-c
    }
}

clebsInstall(xerces) {
    win32 {
        CONFIG(debug, debug|release) {
            xercesinstall.files = $${XERCESCDLLDIR_DEBUG}/$${XERCESCDLL_DEBUG}
        } else {
            xercesinstall.files = $${XERCESCDLLDIR_RELEASE}/$${XERCESCDLL_RELEASE}
        }
        xercesinstall.path = $$BINDIR
        INSTALLS *= xercesinstall
    }
}
