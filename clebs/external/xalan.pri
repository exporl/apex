clebsCheck(xalan) {
    win32 {
        isEmpty(XALANCROOT):XALANCROOT = $$BASEDIR/../api/xalan
        isEmpty(XALANCLIB):XALANCLIB = xalan-c_1
        isEmpty(XALANCLIB_RELEASE):XALANCLIB_RELEASE = $${XALANCLIB}
        isEmpty(XALANCLIB_DEBUG):XALANCLIB_DEBUG = $${XALANCLIB}d
        isEmpty(XALANCLIBDIR_RELEASE):XALANCLIBDIR = $${XALANCROOT}/build/win32/vc7.1
        isEmpty(XALANCLIBDIR_RELEASE):XALANCLIBDIR_RELEASE = $${XALANCLIBDIR}/release
        isEmpty(XALANCLIBDIR_DEBUG):XALANCLIBDIR_DEBUG = $${XALANCLIBDIR}/debug
        isEmpty(XALANCDLLS):XALANCDLLS = xalan-c_1_10 xalanmessages_1_10
        isEmpty(XALANCDLLS_RELEASE):for(i,XALANCDLLS):XALANCDLLS_RELEASE *= $${i}.dll
        isEmpty(XALANCDLLS_DEBUG):for(i,XALANCDLLS):XALANCDLLS_DEBUG *= $${i}d.dll
        isEmpty(XALANCDLLDIR):XALANCDLLDIR = $${XALANCLIBDIR}
        isEmpty(XALANCDLLDIR_RELEASE):XALANCDLLDIR_RELEASE = $${XALANCDLLDIR}/release
        isEmpty(XALANCDLLDIR_DEBUG):XALANCDLLDIR_DEBUG = $${XALANCDLLDIR}/debug
        isEmpty(XALANCINCLUDEDIR):XALANCINCLUDEDIR = $${XALANCROOT}/src

        exists ($${XALANCROOT}):CLEBS_DEPENDENCIES *= xalan
    }

    unix {
        exists (/usr/include/xalanc):CLEBS_DEPENDENCIES *= xalan
    }
}

clebsDependency(xalan) {
    win32 {
        INCLUDEPATH *= $${XALANCINCLUDEDIR}
        CONFIG(debug, debug|release) {
            LIBS *= -L$${XALANCLIBDIR_DEBUG} -l$${XALANCLIB_DEBUG}
        } else {
            LIBS *= -L$${XALANCLIBDIR_RELEASE} -l$${XALANCLIB_RELEASE}
        }
    }

    unix {
        LIBS *= -lxalan-c
    }
}

clebsInstall(xalan) {
    win32 {
        CONFIG(debug, debug|release) {
            for(i,XALANCDLLS_DEBUG):xalaninstall.files *= $${XALANCDLLDIR_DEBUG}/$$i
        } else {
            for(i,XALANCDLLS_RELEASE):xalaninstall.files *= $${XALANCDLLDIR_RELEASE}/$$i
        }
        xalaninstall.path = $$BINDIR
        INSTALLS *= xalaninstall
    }
}
