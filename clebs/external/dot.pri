clebsCheck(dot) {
    win32 {
        isEmpty(DOTROOT):DOTROOT = $$BASEDIR/../api/dot
        isEmpty(DOTBINDIR):DOTBINDIR = $${DOTROOT}/bin

        exists($${DOTBINDIR}/dot.exe):CLEBS_DEPENDENCIES *= dot
    }

    unix {
    }
}

clebsInstall(dot) {
    win32 {
        installfiles *= dot.exe gvc.dll cdt.dll ltdl.dll zlib1.dll Pathplan.dll cgraph.dll libexpat.dll gvplugin_core.dll gvplugin_dot_layout.dll config6
        for(installfile, installfiles):dotbininstall.files += $${DOTBINDIR}/$${installfile}
        dotbininstall.path = $${BINDIR}/dot
        INSTALLS *= dotbininstall
    }
}
