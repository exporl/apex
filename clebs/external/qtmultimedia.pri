clebsCheck(qtmultimedia) {
    clebsCheckQtVersion(5, 0, 0):qtHaveModule(multimedia):CLEBS_DEPENDENCIES *= qtmultimedia
}

clebsDependency(qtmultimedia) {
    QT *= multimedia
}
