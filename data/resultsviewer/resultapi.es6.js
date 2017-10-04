/*jshint esversion:6, curly: false, devel: true, browser: true */

class ResultApi
{
    constructor(url, token)
    {
        this._url = url;
        this._token = token;
        this._socket = new CommonSocket(url, token, this._onMessage);
        this._socket.on('loadHtml', this._loadHtml);
        this._socket.on('evaluateJavaScript', this._evaluateJavaScript);
        this._socket.open();
    }

    psignifit(data)
    {
        return this._socket.sendBlocking(
            this._socket.buildInvokeMessage('psignifit', [ data ]));
    }

    exportToPdf()
    {
        this._socket.sendAsync(this._socket.buildInvokeMessage('exportToPdf', []));
    }

    on(method, cb)
    {
        this._socket.on(method, cb);
    }

    _onMessage(msg)
    {
    }

    _loadHtml(data) {
        document.documentElement.outerHTML = data;
    }

    _evaluateJavaScript(script){
        eval(script);
    }
}
