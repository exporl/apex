QT *= widgets network

!android:contains(DEFINES, WITH_WEBENGINE) {
    QT *= webenginewidgets
} else:!android {
    QT *=  webkitwidgets
}

android:QT *= qml quick webview quickwidgets
