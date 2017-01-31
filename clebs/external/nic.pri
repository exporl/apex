clebsCheck(nic) {
    win32 {
	isEmpty(NUCLEUSROOT):NUCLEUSROOT = $$BASEDIR/../api/nucleus
	isEmpty(NUCLEUSLIBDIR):NUCLEUSLIBDIR = $${NUCLEUSROOT}/libraries/msvc/v7.1
	isEmpty(NUCLEUSINCLUDEDIR):NUCLEUSINCLUDEDIR = $${NUCLEUSROOT}/include

	exists($${NUCLEUSINCLUDEDIR}):CLEBS_DEPENDENCIES *= nic
    }

    unix {
    }
}

clebsDependency(nic) {
    win32 {
	INCLUDEPATH *= $${NUCLEUSINCLUDEDIR}
	LIBS *= -L$${NUCLEUSLIBDIR} -lNIC -lservices -lsockets
    }

    unix {
    }
}
