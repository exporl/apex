CLEBS_INSTALL *= portaudio fftw3 fftw3f iowkit sndfile xml2 zlib protobuf dot liblo libgit2 libssh2 openssl

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

js.path = $$DATADIR/js
js.files = js/*.js

xslt.path = $$DATADIR/xslt
xslt.files = xslt/*.xsl

tests_libapex.path = $$DATADIR/tests/libapex
tests_libapex.files = tests/libapex/*

tests_libwriters.path = $$DATADIR/tests/libwriters
tests_libwriters.files = tests/libwriters/*

icons.path = $$ICONDIR
icons.files = icons/*.png icons/*.ico icons/*.xpm

INSTALLS *= schemas config plugins scripts xslt tests_libapex tests_libwriters icons js

unix {
    desktopfile.files = exporl-apex.desktop
    desktopfile.path = $$APPDIR
    INSTALLS *= desktopfile
}

win32 {
    qt5.path = $$BINDIR
    CONFIG(debug, debug|release) {
        installfiles *= Qt5Xmld.dll Qt5Cored.dll Qt5Guid.dll Qt5Svgd.dll
        installfiles *= Qt5Networkd.dll Qt5Scriptd.dll Qt5WebChanneld.dll
        installfiles *= Qt5XmlPatternsd.dll Qt5Testd.dll
        installfiles *= Qt5ScriptToolsd.dll
        installfiles *= Qt5Widgetsd.dll libGLESv2d.dll Qt5WebSocketsd.dll
        installfiles *= Qt5MultimediaWidgetsd.dll Qt5Multimediad.dll Qt5OpenGLd.dll
        installfiles *= Qt5PrintSupportd.dll Qt5Quickd.dll Qt5Qmld.dll
        installfiles *= Qt5Sqld.dll Qt5Positioningd.dll Qt5Sensorsd.dll
        installfiles *= Qt5QuickWidgetsd.dll Qt5WebEngineCored.dll Qt5WebEngined.dll Qt5WebEngineWidgetsd.dll
        installfiles *= libEGLd.dll
    } else {
        installfiles *= Qt5Xml.dll Qt5Core.dll Qt5Gui.dll Qt5Svg.dll
        installfiles *= Qt5Network.dll Qt5Script.dll Qt5WebChannel.dll
        installfiles *= Qt5XmlPatterns.dll Qt5Test.dll
        installfiles *= Qt5ScriptTools.dll
        installfiles *= Qt5Widgets.dll libGLESv2.dll Qt5WebSockets.dll
        installfiles *= Qt5MultimediaWidgets.dll Qt5Multimedia.dll Qt5OpenGL.dll
        installfiles *= Qt5PrintSupport.dll Qt5Quick.dll Qt5Qml.dll
        installfiles *= Qt5Sql.dll Qt5Positioning.dll Qt5Sensors.dll
        installfiles *= Qt5QuickWidgets.dll Qt5WebEngineCore.dll Qt5WebEngine.dll Qt5WebEngineWidgets.dll
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
        qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qtiffd.dll
        qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qsvgd.dll
        qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qjpegd.dll
    } else {
        qt5imageformats.files += $$[QT_INSTALL_PLUGINS]/imageformats/qgif.dll
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

    CONFIG(debug, debug|release) {
        qt5webengine.files += $$[QT_INSTALL_BINS]/QtWebEngineProcessd.exe
    } else {
        qt5webengine.files += $$[QT_INSTALL_BINS]/QtWebEngineProcess.exe
    }
    qt5webengine.path = $$BINDIR
    INSTALLS *= qt5webengine

    qt5webengineresources.files += $$[QT_INSTALL_PREFIX]/resources/qtwebengine_*.pak $$[QT_INSTALL_PREFIX]/resources/icudtl.dat
    qt5webengineresources.path = $$BINDIR
    INSTALLS *= qt5webengineresources

    qt5webenginetranslations.files += $$[QT_INSTALL_TRANSLATIONS]/qtwebengine_locales/en-*.pak $$[QT_INSTALL_TRANSLATIONS]/qtwebengine_locales/nl.pak
    qt5webenginetranslations.path = $$BINDIR/qtwebengine_locales
    INSTALLS *= qt5webenginetranslations
}
