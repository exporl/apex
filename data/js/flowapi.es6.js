/*jshint esversion:6, curly: false, devel: true, browser: true */
/* eslint-disable no-unused-vars, no-undef */

class FlowApi {
    constructor(url, token) {
        this._url = url;
        this._token = token;
        this._socket = new CommonSocket(url, token, this._onMessage);
        this._socket.on('loadHtml', this._loadHtml);
        this._socket.on('evaluateJavaScript', this._evaluateJavaScript);
        this._socket.open();
    }

    runExperiment(path, expressions = {}, results = '') {
        let promises = [];
        for (var key in expressions) {
            if (expressions.hasOwnProperty(key)) {
                var expression = this._socket.buildInvokeMessage('addExpression',
                                                                 [key, expressions[key]]);
                promises.push(this._socket.sendBlocking(expression));
            }
        }
        return Promise.all(promises).then(() => {
            return this._socket.sendBlocking(this._socket.buildInvokeMessage(
                'runExperiment', [ path, results ]), 10000);
        });
    }

    absoluteFilePath(path) {
        let msg = this._socket.buildInvokeMessage('absoluteFilePath', [path]);
        return this._socket.sendBlocking(msg);
    }

    fromCurrentDir(path) {
        console.error('FromCurrentDir is deprecated, use absoluteFilePath instead');
        return this.absoluteFilePath(path);
    }

    readFile(path) {
        let msg = this._socket.buildInvokeMessage('readFile', [path]);
        return this._socket.sendBlocking(msg);
    }

    writeFile(path, content) {
        let msg = this._socket.buildInvokeMessage('writeFile', [path, content]);
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

    _evaluateJavaScript(script){
        eval(script);
    }
}
