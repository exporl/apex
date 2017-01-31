clebsCheck(controllerplugin) {
    CLEBS_DEPENDENCIES *= controllerplugin
}

clebsDependency(controllerplugin) {
    INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/device
    DEPENDPATH *= $$BASEDIR/src/lib/apexmain/device
}
