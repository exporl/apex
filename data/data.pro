CLEBS_INSTALL *= portaudio fftw3 fftw3f iowkit sndfile xml2 zlib protobuf dot liblo

!isEmpty(_PRO_FILE_):include(../clebs/clebs.pri)

TEMPLATE = subdirs

schemas.path = $$DATADIR/schemas
schemas.files = schemas/*.xsd

config.path = $$DATADIR/config
config.files = config/*.xml

plugins.path = $$DATADIR/pluginprocedures
plugins.files = pluginprocedures/*

scripts.path = $$DATADIR/resultsviewer
scripts.files = resultsviewer/*

xslt.path = $$DATADIR/xslt
xslt.files = xslt/*.xsl

tests_libapex.path = $$DATADIR/tests/libapex
tests_libapex.files = tests/libapex/*

tests_libwriters.path = $$DATADIR/tests/libwriters
tests_libwriters.files = tests/libwriters/*

icons.path = $$ICONDIR
icons.files = icons/*.png icons/*.ico icons/*.xpm

INSTALLS *= schemas config plugins scripts xslt tests_libapex tests_libwriters icons

unix {
    desktopfile.files = exporl-apex.desktop
    desktopfile.path = $$APPDIR
    INSTALLS *= desktopfile
}

win32 {
    equals(QT_MAJOR_VERSION, 4) {
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
    }

    equals(QT_MAJOR_VERSION, 5) {
       qt5.path = $$BINDIR
       CONFIG(debug, debug|release) {
            installfiles *= Qt5Xmld.dll Qt5Cored.dll Qt5Guid.dll Qt5Svgd.dll
            installfiles *= Qt5Networkd.dll Qt5Scriptd.dll Qt5WebKitd.dll Qt5WebChanneld.dll
            installfiles *= Qt5AxContainerd.dll Qt5XmlPatternsd.dll Qt5Testd.dll
            installfiles *= Qt5ScriptToolsd.dll
            installfiles *= icuin54.dll icuuc54.dll icudt54.dll icuin51.dll icuuc51.dll icudt51.dll
            installfiles *= Qt5Widgetsd.dll libGLESv2d.dll Qt5WebkitWidgetsd.dll
            installfiles *= Qt5MultimediaWidgetsd.dll Qt5Multimediad.dll Qt5OpenGLd.dll
            installfiles *= Qt5PrintSupportd.dll Qt5Quickd.dll Qt5Qmld.dll
            installfiles *= Qt5Sqld.dll Qt5Positioningd.dll Qt5Sensorsd.dll
            installfiles *= libEGLd.dll
        } else {
            installfiles *= Qt5Xml.dll Qt5Core.dll Qt5Gui.dll Qt5Svg.dll
            installfiles *= Qt5Network.dll Qt5Script.dll Qt5WebKit.dll Qt5WebChannel.dll
            installfiles *= Qt5AxContainer.dll Qt5XmlPatterns.dll Qt5Test.dll
            installfiles *= Qt5ScriptTools.dll
            installfiles *= icuin54.dll icuuc54.dll icudt54.dll icuin51.dll icuuc51.dll icudt51.dll
            installfiles *= Qt5Widgets.dll libGLESv2.dll Qt5WebkitWidgets.dll
            installfiles *= Qt5MultimediaWidgets.dll Qt5Multimedia.dll Qt5OpenGL.dll
            installfiles *= Qt5PrintSupport.dll Qt5Quick.dll Qt5Qml.dll
            installfiles *= Qt5Sql.dll Qt5Positioning.dll Qt5Sensors.dll
            installfiles *= libEGL.dll
        }
        win32-g++|win32-x-g++*:installfiles *= mingwm10.dll libgcc_s_dw2-1.dll
        win32-x-g++-4.2:installfiles *= libgcc_sjlj_1.dll libstdc++_sjlj_6.dll

        # all lib paths (will also work for cross-compiles)
        installpaths *= $$[QT_INSTALL_BINS] $${QMAKE_LIBDIR_QT}
        for(installfile, installfiles) {
        for(installpath, installpaths) {
                exists($${installpath}/$${installfile}) {
                qt5.files *= $${installpath}/$${installfile}
                break()
                }
            }
        }

        INSTALLS *= qt5

        # qt image plugins
        CONFIG(debug, debug|release) {
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qgifd.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qmngd.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qtiffd.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qsvgd.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qjpegd.dll
        } else {
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qgif.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qmng.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qtiff.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qsvg.dll
            qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qjpeg.dll
        }
        qt5imageformats.path = $$BINDIR/imageformats

        INSTALLS *= qt5imageformats

        # qt platform plugins
        CONFIG(debug, debug|release) {
            qt5platform.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindowsd.dll
        } else {
            qt5platform.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
        }
        qt5platform.path = $$BINDIR/platforms

        INSTALLS *= qt5platform
    }


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
