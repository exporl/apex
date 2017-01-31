clebsCheck(chrpath) {
    win32 {
    }

    unix {
	system(which chrpath 2>&1 > /dev/null):CLEBS_DEPENDENCIES *= chrpath
    }
}

clebsDependency(chrpath) {
    win32 {
    }

    unix {
	QMAKE_RPATHDIR *= $$DESTDIR

	removerpath.path = $$target.path
	contains (TEMPLATE, lib) {
	    removerpath.extra = chrpath -d $(INSTALL_ROOT)/$$target.path/$(TARGET)
	    INSTALLS *= removerpath
	}
	contains (TEMPLATE, app) {
	    removerpath.extra = chrpath -d $(INSTALL_ROOT)/$$target.path/$(QMAKE_TARGET)
	    INSTALLS *= removerpath
	}
    }
}
