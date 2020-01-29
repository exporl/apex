import QtQuick 2.3
import QtWebView 1.1

Rectangle {
    id: root

    signal loadingFinished(bool ok)
    signal javaScriptFinished(variant result)

    WebView {
        id: webView
        anchors.fill: parent

        onLoadingChanged: {
            if (loadRequest.errorString)
                console.error(loadRequest.errorString);
            if (loadRequest.status === WebView.LoadSucceededStatus)
                root.loadingFinished(true);
        }
    }

    function load(url) {
        webView.url = url;
    }

    function loadHtml(html) {
        webView.loadHtml(html, "");
    }

    function runJavaScript(script) {
        webView.runJavaScript(script);
    }

    function runJavaScriptAndEmitResult(script) {
        webView.runJavaScript(script, root.javaScriptFinished);
    }
}
