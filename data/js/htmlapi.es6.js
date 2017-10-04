/*jshint esversion:6, curly: false, devel: true, browser: true */

class HtmlApi {
    constructor(url, token) {
        this._url = url;
        this._token = token;
        this._socket = new CommonSocket(url, token, this._onMessage);
        this._socket.on('loadHtml', this._loadHtml);
        this._socket.on('evaluateJavaScript', this._evaluateJavaScript);
        this._socket.open();
    }

    on(method, cb) {
        this._socket.on(method, cb);
    }

    answered() {
        let msg = this._socket.buildInvokeMessage('answered', []);
        return this._socket.sendBlocking(msg);

    }

    _onMessage(msg) {
    }

    _loadHtml(data) {
        document.documentElement.outerHTML = data;
    }

    _evaluateJavaScript(script){
        eval(script);
    }
}
