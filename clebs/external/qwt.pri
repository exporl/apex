clebsCheck(qwt) {
    win32 {
	isEmpty(QWTROOT):QWTROOT = $$BASEDIR/../api/qwt
	isEmpty(QWTLIB):QWTLIB = qwt5
	isEmpty(QWTLIB_RELEASE):QWTLIB_RELEASE = $${QWTLIB}
	isEmpty(QWTLIB_DEBUG):QWTLIB_DEBUG = $${QWTLIB}
	isEmpty(QWTLIBDIR):QWTLIBDIR = $${QWTROOT}/lib
	isEmpty(QWTLIBDIR_RELEASE):QWTLIBDIR_RELEASE = $${QWTLIBDIR}/release
	isEmpty(QWTLIBDIR_DEBUG):QWTLIBDIR_DEBUG = $${QWTLIBDIR}/debug
	isEmpty(QWTINCLUDEDIR):QWTINCLUDEDIR = $${QWTROOT}/include

	exists($${QWTINCLUDEDIR}):CLEBS_DEPENDENCIES *= qwt
    }

    unix {
	isEmpty(QWTLIB):QWTLIB = qwt-qt4
	isEmpty(QWTINCLUDEDIR):QWTINCLUDEDIR = /usr/include/qwt-qt4

	exists($${QWTINCLUDEDIR}):CLEBS_DEPENDENCIES *= qwt
    }
}

clebsDependency(qwt) {
    win32 {
	INCLUDEPATH *= $${QWTINCLUDEDIR}
	CONFIG(debug, debug|release) {
	    LIBS *= -L$${QWTLIBDIR_DEBUG} -l$${QWTLIB_DEBUG}
	} else {
	    LIBS *= -L$${QWTLIBDIR_RELEASE} -l$${QWTLIB_RELEASE}
	}
	DEFINES *= QWT_DLL
    }
    unix {
	INCLUDEPATH *= $${QWTINCLUDEDIR}
	LIBS *= -l$${QWTLIB}
    }
}

clebsInstall(qwt) {
    win32 {
	CONFIG(debug, debug|release) {
	    qwtinstall.files = $${QWTLIBDIR_DEBUG}/$${QWTLIB_DEBUG}.dll
	} else {
	    qwtinstall.files = $${QWTLIBDIR_RELEASE}/$${QWTLIB_RELEASE}.dll
	}
	qwtinstall.path = $$BINDIR
	INSTALLS *= qwtinstall
    }
}
