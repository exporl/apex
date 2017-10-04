import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtWebView 1.1

Rectangle {
    id: root
    signal loadingFinished()
    signal javascriptFinished(variant result)

    WebView {
        id: webView
        anchors.fill: parent

        onLoadingChanged: {
            if (loadRequest.errorString)
                console.error(loadRequest.errorString);
            if (loadRequest.status === WebView.LoadSucceededStatus)
                root.loadingFinished()
        }
    }

    function load(url) {
        webView.url = url;
    }

    function loadHtml(html) {
        webView.loadHtml(html, "");
    }

    function runJavaScript(script) {
        webView.runJavaScript(script, function(result) {
            root.javascriptFinished(result);
        });
    }
}
