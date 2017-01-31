BASEDIR = ..
CLEBS_INSTALL *= xalan xerces portaudio fftw3 iowkit sndfile
include($$BASEDIR/clebs.pri)

OBJECTS_DIR =
TEMPLATE = subdirs

schemas.path = $$DATADIR/schemas
schemas.files = schemas/*.xsd

config.path = $$CONFDIR
config.files = config/*.xml 

plugins.path = $$DATADIR/plugins
plugins.files = plugins/*

scripts.path = $$DATADIR/scripts
scripts.files = scripts/*

xslt.path = $$DATADIR/xslt
xslt.files = xslt/*.xsl

data.path = $$DATADIR/data
data.files = data/*.png data/*.ico data/*.xpm

icons.files = icons/*
icons.path = $$ICONDIR

INSTALLS *= icons schemas config plugins scripts xslt data

unix {
    desktopfile.files = exporl-apex.desktop
    desktopfile.path = $$APPDIR
    INSTALLS *= desktopfile
}

win32 {

   qt4.path = $$BINDIR
   CONFIG(debug, debug|release) {
        installfiles *= QtXmld4.dll QtCored4.dll QtGuid4.dll QtSvgd4.dll
        installfiles *= QtNetworkd4.dll QtScriptd4.dll QtWebKitd4.dll phonond4.dll
        installfiles *= QAxContainerd.dll QtXmlPatternsd4.dll QtTestd4.dll
        installfiles *= QtScriptToolsd4.dll
    } else {
        installfiles *= QtXml4.dll QtCore4.dll QtGui4.dll QtSvg4.dll
        installfiles *= QtNetwork4.dll QtScript4.dll QtWebKit4.dll phonon4.dll
        installfiles *= QAxContainer.dll QtXmlPatterns4.dll QtTest4.dll
        installfiles *= QtScriptTools4.dll
    }
    win32-g++|win32-x-g++*:installfiles *= mingwm10.dll libgcc_s_dw2-1.dll
    win32-x-g++-4.2:installfiles *= libgcc_sjlj_1.dll libstdc++_sjlj_6.dll
    
    # all lib paths (will also work for cross-compiles)
    installpaths *= $$[QT_INSTALL_BINS] $${QMAKE_LIBDIR_QT} 
    for(installfile, installfiles) {
  	for(installpath, installpaths) {
    	    exists($${installpath}/$${installfile}) {
    		qt4.files *= $${installpath}/$${installfile}
    		break()
    	    }
    	}
    }
    
    INSTALLS *= qt4
    
    # qt image plugins
    CONFIG(debug, debug|release) {
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qgif4d.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qmng4d.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qtiff4d.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qsvg4d.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qjpeg4d.dll
	} else {
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qgif4.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qmng4.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qtiff4.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qsvg4.dll
	    qt4imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qjpeg4.dll
	}
	qt4imageformats.path = $$BINDIR/imageformats

    INSTALLS *= qt4imageformats
    
    
    win32-vs2008: {
        # MFC
        WINDOWSDIR = c:/windows/system32

        mfc.files += $${WINDOWSDIR}/mfc71.dll
        mfc.files += $${WINDOWSDIR}/MFC71u.dll
        mfc.files += $${WINDOWSDIR}/msvcp71.dll
        mfc.files += $${WINDOWSDIR}/msvcr71d.dll
        mfc.files += $${WINDOWSDIR}/msvcr71.dll

        mfc.path = $$BINDIR
        INSTALLS *= mfc
    }

    win32-msvc2010: {
        # MFC
        WINDOWSDIR = c:/windows/system32

        mfc.files += $${WINDOWSDIR}/msvcp100.dll
        mfc.files += $${WINDOWSDIR}/msvcr100d.dll
        mfc.files += $${WINDOWSDIR}/msvcr100.dll

        mfc.path = $$BINDIR
        INSTALLS *= mfc
    }

}
