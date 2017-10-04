clebsCheck(xml2) {
    win32 {
        isEmpty(XML2ROOT):XML2ROOT = $$BASEDIR/../api/xml2
        isEmpty(XML2BINDIR):XML2BINDIR = $${XML2ROOT}/bin
        isEmpty(XML2LIBDIR):XML2LIBDIR = $${XML2ROOT}/lib
        isEmpty(XML2LIB):XML2LIB = libxml2

        isEmpty(ICONVROOT):ICONVROOT = $$BASEDIR/../api/iconv
        isEmpty(ICONVBINDIR):ICONVBINDIR = $${ICONVROOT}/bin
        isEmpty(ICONVLIBDIR):ICONVLIBDIR = $${ICONVROOT}/lib
        isEmpty(ICONVLIB):ICONVLIB = iconv

        exists($${XML2ROOT}):CLEBS_DEPENDENCIES *= xml2
    }

    unix {
       system(pkg-config libxml-2.0):CLEBS_DEPENDENCIES *= xml2
    }
}

clebsDependency(xml2) {
    win32 {
        INCLUDEPATH *= $${XML2ROOT}/include
        LIBS *= -L$${XML2LIBDIR} -l$${XML2LIB}

        INCLUDEPATH *= $${ICONVROOT}/include
        LIBS *= -L$${ICONVLIBDIR} -l$${ICONVLIB}
    }

    unix {
        PKGCONFIG *= libxml-2.0
    }
}

clebsInstall(xml2) {
    win32 {
        xml2install.files = $${XML2BINDIR}/$${XML2LIB}.dll
        xml2install.files += $${ICONVBINDIR}/$${ICONVLIB}.dll

        xml2install.path = $$BINDIR
        INSTALLS *= xml2install
    }
}
