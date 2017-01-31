clebsCheck(audiofile) {
    win32 {
    }

    unix {
	system(pkg-config audiofile):CLEBS_DEPENDENCIES *= audiofile
    }
}

clebsDependency(audiofile) {
    win32 {
    }

    unix {
	PKGCONFIG *= audiofile
    }
}
