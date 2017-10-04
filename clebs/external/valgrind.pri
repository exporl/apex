clebsCheck(valgrind) {
    win32 {
    }

    unix {
        isEmpty(VALGRINDINCLUDEDIR):VALGRINDINCLUDEDIR = /usr/include/valgrind
        exists($${VALGRINDINCLUDEDIR}):CLEBS_DEPENDENCIES *= valgrind

        # The pkg-config configuration for valgrind tries to link against
        # coregrind, which does not exist
        # system(pkg-config valgrind):CLEBS_DEPENDENCIES *= valgrind
    }
}

clebsDependency(valgrind) {
    win32 {
    }

    unix {
        INCLUDEPATH *= $${VALGRINDINCLUDEDIR}
    }
}

clebsInstall(valgrind) {
    win32 {
    }
}

