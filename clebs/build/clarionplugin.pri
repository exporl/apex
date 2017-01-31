clebsCheck(clarionplugin) {
    CLEBS_DEPENDENCIES *= clarionplugin
}

clebsDependency(clarionplugin) {
    INCLUDEPATH *= $$BASEDIR/src/apex/stimulus/clarion
    DEPENDPATH *= $$BASEDIR/src/apex/stimulus/clarion
}
