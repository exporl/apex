clebsCheck(pch) {
    CLEBS_DEPENDENCIES *= pch
}

clebsDependency(pch) {
    CONFIG += precompile_header
    PRECOMPILED_HEADER = pch.h
    HEADERS *= pch.h
}
