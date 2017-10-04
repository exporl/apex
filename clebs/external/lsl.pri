clebsCheck(lsl) {
    win32 {
        isEmpty(LSLROOT):LSLROOT = $$BASEDIR/../api/lsl
        isEmpty(LSLINCLUDEDIR):LSLINCLUDEDIR = $${LSLROOT}
        isEmpty(LSLLIBDIR):LSLLIBDIR = $${LSLROOT}

        exists($${LSLINCLUDEDIR}):CLEBS_DEPENDENCIES *= lsl
    }

    unix {
        isEmpty(LSLROOT):LSLROOT = /usr/local
        isEmpty(LSLINCLUDEDIR):LSLINCLUDEDIR = $${LSLROOT}/include
        isEmpty(LSLLIBDIR):LSLLIBDIR = $${LSLROOT}/lib

        exists($${LSLINCLUDEDIR}/lsl_cpp.h):CLEBS_DEPENDENCIES *= lsl
    }
}

clebsDependency(lsl) {
    INCLUDEPATH *= $${LSLINCLUDEDIR}

    win32 {
        LIBS *= -L$${LSLLIBDIR} -lliblsl32
    }
    unix {
        LIBS *= -L$${LSLLIBDIR} -llsl
    }
}

clebsInstall(lsl) {
    win32 {
        lslinstall.files = $${LSLLIBDIR}/liblsl32.dll
        lslinstall.path = $$BINDIR
        INSTALLS *= lslinstall
    }
}


