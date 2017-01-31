clebsCheck(zlib) {
    win32 {
	isEmpty(ZLIBROOT):ZLIBROOT = $$BASEDIR/../api/zlib
	isEmpty(ZLIBINCLUDEDIR):ZLIBINCLUDEDIR = $${ZLIBROOT}

	exists($${ZLIBINCLUDEDIR}):CLEBS_DEPENDENCIES *= zlib
    }

    unix {
	CLEBS_DEPENDENCIES *= zlib
    }
}

clebsDependency(zlib) {
    win32 {
	# Zlib(included with Qt, we only need the header)
	INCLUDEPATH *= $${ZLIBINCLUDEDIR}
    }

    unix {
    }
}
