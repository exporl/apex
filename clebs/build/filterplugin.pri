clebsCheck(filterplugin) {
    CLEBS_DEPENDENCIES *= filterplugin
}

clebsDependency(filterplugin) {
    INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/filter
    DEPENDPATH *= $$BASEDIR/src/lib/apexmain/filter
}
