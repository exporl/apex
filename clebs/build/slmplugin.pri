clebsCheck(slmplugin) {
    CLEBS_DEPENDENCIES *= slmplugin
}

clebsDependency(slmplugin) {
    INCLUDEPATH *= $$BASEDIR/src/lib/apexmain/
    DEPENDPATH *= $$BASEDIR/src/lib/apexmain/
}
