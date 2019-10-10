clebsCheck(nic4) {
    win32 {
        isEmpty(NIC4ROOT):NIC4ROOT = $$BASEDIR/../api/nic4
        isEmpty(NIC4BINDIR):NIC4BINDIR = $${NIC4ROOT}/binaries
        isEmpty(NIC4LIBDIR):NIC4LIBDIR = $${NIC4ROOT}/c/lib/Win32
        isEmpty(NIC4INCLUDEDIR):NIC4INCLUDEDIR = $${NIC4ROOT}/c/include

        exists($${NIC4INCLUDEDIR}):CLEBS_DEPENDENCIES *= nic4
    }

    unix {
        isEmpty(NIC4ROOT):NIC4ROOT = /opt/nucleus/nic4
        isEmpty(NIC4LIBDIR):NIC4LIBDIR = $${NIC4ROOT}/lib
        isEmpty(NIC4INCLUDEDIR):NIC4INCLUDEDIR = $${NIC4ROOT}/include/nic4

        exists($${NIC4INCLUDEDIR}):CLEBS_DEPENDENCIES *= nic4
    }
}

clebsDependency(nic4) {
    win32 {
        INCLUDEPATH *= $${NIC4INCLUDEDIR}
        LIBS *= -L$${NIC4LIBDIR} -lnic4
    }

    unix {
        INCLUDEPATH *= $${NIC4INCLUDEDIR}
        LIBS *= -L$${NIC4LIBDIR} -lnic
    }
}

clebsInstall(nic4) {
    win32 {
        installfiles *= nic4.dll

        for(installfile, installfiles) {
            nic4install.files *= $$NIC4BINDIR/$${installfile}
        }
        nic4install.path = $$BINDIR

        INSTALLS *= nic4install
    }
}
