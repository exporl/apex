clebsCheck(runnerplugin) {
    CLEBS_DEPENDENCIES *= runnerplugin
}

clebsDependency(runnerplugin) {
    INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/runner
    DEPENDPATH *= $$BASEDIR/src/lib/apexmain/runner
}
