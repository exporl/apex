/*jshint esversion:6, curly: false, devel: true, browser: true */

class ResultApi {
    constructor(url, token) {
        this._url = url;
        this._token = token;
        this._socket = new CommonSocket(url, token, this._onMessage);
        this._socket.on('loadHtml', this._loadHtml);
        this._socket.on('evaluateJavaScript', this._evaluateJavaScript);
        this._socket.open();
    }

    psignifit(data) {
        return this._socket.sendBlocking(
            this._socket.buildInvokeMessage('psignifit', [ data ]));
    }

    exportToPdf() {
        this._socket.sendAsync(
            this._socket.buildInvokeMessage('exportToPdf', []));
    }

    absoluteFilePath(path) {
        let msg = this._socket.buildInvokeMessage('absoluteFilePath', [ path ]);
        return this._socket.sendBlocking(msg);
    }

    readFile(path) {
        let msg = this._socket.buildInvokeMessage('readFile', [ path ]);
        return this._socket.sendBlocking(msg);
    }

    onReady(callback) {
        this._socket.onOpen(callback);
    }

    on(method, cb) {
        this._socket.on(method, cb);
    }

    loadScript(path) {
        return this.absoluteFilePath(path).then((absolutePath) => {
            return this.readFile(absolutePath).then((content) => {
                let scriptElement = document.createElement('script');
                scriptElement.setAttribute('type', 'text/javascript');
                scriptElement.textContent = content;
                return new Promise((resolve, reject) => {
                    scriptElement.onload = resolve();
                    document.head.appendChild(scriptElement);
                    setTimeout(() => {
                        reject('Script loading timed out');
                    }, 5000);
                });
            });
        });
    }

    _onMessage(msg) {
    }

    _loadHtml(data) {
        document.documentElement.outerHTML = data;
    }

    _evaluateJavaScript(script) {
        eval(script);
    }
}
