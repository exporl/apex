clebsCheck(feedbackplugin) {
    CLEBS_DEPENDENCIES *= feedbackplugin
}

clebsDependency(feedbackplugin) {
    INCLUDEPATH *= $$BASEDIR/src/lib/apexmain
    DEPENDPATH *= $$BASEDIR/src/lib/apexmain
}
