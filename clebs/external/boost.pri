clebsCheck(boost) {
    win32 {
        isEmpty(BOOSTROOT):BOOSTROOT = $$BASEDIR/../api/boost
        isEmpty(BOOSTINCLUDEDIR):BOOSTINCLUDEDIR = $${BOOSTROOT}

        exists($${BOOSTINCLUDEDIR}/boost):CLEBS_DEPENDENCIES *= boost
    }

    unix {
        isEmpty(BOOSTINCLUDEDIR) {
            exists(/usr/include/boost):CLEBS_DEPENDENCIES *= boost
        } else {
            exists($${BOOSTINCLUDEDIR}/boost):CLEBS_DEPENDENCIES *= boost
        }
    }
}

clebsDependency(boost) {
    win32 {
        INCLUDEPATH *= $${BOOSTINCLUDEDIR}
    }
    unix {
        INCLUDEPATH *= $${BOOSTINCLUDEDIR}
    }
}
