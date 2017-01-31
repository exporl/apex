clebsCheck(jack) {
    win32 {
    }

    unix {
	system(pkg-config jack):CLEBS_DEPENDENCIES *= jack
    }
}

clebsDependency(jack) {
    win32 {
    }
    unix {
	PKGCONFIG *= jack
    }
}
