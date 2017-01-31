clebsCheck(iowkit) {
    win32 {
	isEmpty(IOWKITROOT):IOWKITROOT = $$BASEDIR/../api/iowkit_1_5
	exists($${IOWKITROOT}/iowkit.h):CLEBS_DEPENDENCIES *= iowkit
    }

    unix {
	isEmpty(IOWKITROOT):IOWKITROOT = /usr/local
	exists($${IOWKITROOT}/include/iowkit.h):CLEBS_DEPENDENCIES *= iowkit
    }
}

clebsDependency(iowkit) {
    win32 {
    	INCLUDEPATH *= $${IOWKITROOT}
    	LIBS *= -L$${IOWKITROOT} -liowkit 
    }

    unix {
    	INCLUDEPATH *= $${IOWKITROOT}/include
    	LIBS *= -L$${IOWKITROOT}/lib -liowkit 
    }
}

clebsInstall(iowkit) {
    win32 {
	iowkitinstall.files = $${IOWKITROOT}/iowkit.dll
	iowkitinstall.path = $$BINDIR
	INSTALLS *= iowkitinstall
    }
}