clebsCheck(matlab) {
    win32 {
        isEmpty(MATLABROOT):MATLABROOT = "c:/Progra~1/MATLAB"
    }
    unix {
        isEmpty(MATLABROOT):MATLABROOT = /opt/matlab
    }
    exists($${MATLABROOT}):CLEBS_DEPENDENCIES *= matlab
}

clebsDependency(matlab) {
    win32 {
        INCLUDEPATH *= $${MATLABROOT}/extern/include
        LIBS *= -L$${MATLABROOT}/bin/win32 -L$${MATLABROOT}/extern/lib/win32/microsoft -llibeng -llibmx -llibmex
    }
    unix {
        INCLUDEPATH *= $${MATLABROOT}/extern/include

        exists( $${MATLABROOT}/bin/glnxa64/MATLAB ) {
#           message("64bit")
            MATLABSUB=glnxa64
        }
        exists( $${MATLABROOT}/bin/glnx86/matlab ) {
#           message("32bit")
            MATLABSUB=glnx86
        }
        LIBS *= -L$${MATLABROOT}/bin/$${MATLABSUB} -leng -lmx -lmex
        # Matlab also contains Qt4, make sure it uses the system Qt4
        QMAKE_LFLAGS *= -Wl,-rpath-link,$${QMAKE_LIBDIR_QT} -Wl,-rpath-link,$${MATLABROOT}/bin/$${MATLABSUB}
    }
}
