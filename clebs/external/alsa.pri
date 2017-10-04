clebsCheck(alsa) {
    win32 {
    }

    unix {
        system(pkg-config alsa):CLEBS_DEPENDENCIES *= alsa
    }
}

clebsDependency(alsa) {
    win32 {
    }

    unix {
        PKGCONFIG *= alsa
    }
}
