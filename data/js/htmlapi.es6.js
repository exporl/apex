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

    absoluteFilePath(path) {
        let msg = this._socket.buildInvokeMessage('absoluteFilePath', [path]);
        return this._socket.sendBlocking(msg);
    }

    readFile(path) {
        let msg = this._socket.buildInvokeMessage('readFile', [path]);
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

    answered() {
        let msg = this._socket.buildInvokeMessage('answered', []);
        this._socket.sendBlocking(msg);
    }

    _onMessage(msg) {
    }

    _loadHtml(data) {
        document.documentElement.outerHTML = data;
    }

    /* Gets executed in context of this._socket */
    _evaluateJavaScript(script){
        let result = eval(script);
        this.sendBlocking(this.buildInvokeMessage(
            'javascriptFinished', [ result ]));
    }
}
