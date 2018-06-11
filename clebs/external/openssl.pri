clebsCheck(openssl) {
    win32 {
        isEmpty(OPENSSLROOT):OPENSSLROOT = $$BASEDIR/../api/libssl
        isEmpty(OPENSSLBINDIR):OPENSSLBINDIR = $${OPENSSLROOT}/bin
        isEmpty(OPENSSLLIBDIR):OPENSSLLIBDIR = $${OPENSSLROOT}/lib
        isEmpty(LIBSSLLIB):LIBSSLLIB = ssleay32
        isEmpty(LIBCRYPTOLIB):LIBCRYPTOLIB = libeay32

        exists($${OPENSSLROOT}/include/openssl/opensslv.h):CLEBS_DEPENDENCIES *= openssl
    }

    unix {
        system(pkg-config libssl):CLEBS_DEPENDENCIES *= openssl
    }
}

clebsDependency(openssl) {
    win32 {
        INCLUDEPATH *= $${OPENSSLROOT}/include
        LIBS *= -L$${OPENSSLLIBDIR} -l$${LIBSSLLIB} -l$${LIBCRYPTOLIB}

    }

    unix {
        PKGCONFIG *= libssl libcrypto
    }
}

clebsInstall(openssl) {
    win32 {
        opensslinstall.files = $${OPENSSLBINDIR}/$${LIBSSLLIB}.dll
        opensslinstall.files += $${OPENSSLBINDIR}/$${LIBCRYPTOLIB}.dll

        opensslinstall.path = $$BINDIR
        INSTALLS *= opensslinstall
    }
}
