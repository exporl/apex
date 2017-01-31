clebsCheck(buildplugin) {
    CLEBS_DEPENDENCIES *= buildplugin
}

clebsDependency(buildplugin) {
    unix:CONFIG += hide_symbols
    CONFIG += plugin
    TEMPLATE = lib
    target.path = $$BINARYPLUGINDIR
    INSTALLS *= target
}
