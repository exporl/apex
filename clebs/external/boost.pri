clebsCheck(boost) {
    win32 {
	isEmpty(BOOSTROOT):BOOSTROOT = $$BASEDIR/../api/boost
	isEmpty(BOOSTINCLUDEDIR):BOOSTINCLUDEDIR = $${BOOSTROOT}/include/boost-1_33_1

	exists($${BOOSTINCLUDEDIR}):CLEBS_DEPENDENCIES *= boost
    }

    unix {
	isEmpty(BOOSTINCLUDEDIR):BOOSTINCLUDEDIR = /usr/include/boost

	exists($${BOOSTINCLUDEDIR}):CLEBS_DEPENDENCIES *= boost
    }
}

clebsDependency(boost) {
    win32 {
	INCLUDEPATH *= $${BOOSTINCLUDEDIR}
    }

    unix {
    }
}
