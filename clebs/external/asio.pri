clebsCheck(asio) {
    win32 {
        isEmpty(ASIOROOT):ASIOROOT = $$BASEDIR/../api/asio

        exists($${ASIOROOT}/common):CLEBS_DEPENDENCIES *= asio
    }

    unix {
    }
}

clebsDependency(asio) {
    win32 {
        INCLUDEPATH *=					\
            $${ASIOROOT}/common				\
            $${ASIOROOT}/host				\
            $${ASIOROOT}/host/pc			\
            $${BASEDIR}/contrib/iasiothiscallresolver	\

        SOURCES *=					\
            $${ASIOROOT}/common/asio.cpp		\
            $${ASIOROOT}/host/asiodrivers.cpp		\
            $${ASIOROOT}/host/pc/asiolist.cpp		\
            $${BASEDIR}/contrib/iasiothiscallresolver/iasiothiscallresolver.cpp

        LIBS *= $$QMAKE_LIBS_CORE

        DEFINES *= WINVER=0x0501
    }

    unix {
    }
}
