'use strict';
/* eslint-disable no-unused-vars, no-undef */

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

var HtmlApi = function () {
    function HtmlApi(url, token) {
        _classCallCheck(this, HtmlApi);

        this._url = url;
        this._token = token;
        this._socket = new CommonSocket(url, token, this._onMessage);
        this._socket.on('loadHtml', this._loadHtml);
        this._socket.on('evaluateJavaScript', this._evaluateJavaScript);
        this._socket.open();
    }

    _createClass(HtmlApi, [{
        key: 'absoluteFilePath',
        value: function absoluteFilePath(path) {
            var msg = this._socket.buildInvokeMessage('absoluteFilePath', [path]);
            return this._socket.sendBlocking(msg);
        }
    }, {
        key: 'readFile',
        value: function readFile(path) {
            var msg = this._socket.buildInvokeMessage('readFile', [path]);
            return this._socket.sendBlocking(msg);
        }
    }, {
        key: 'writeFile',
        value: function writeFile(path, content) {
            var msg = this._socket.buildInvokeMessage('writeFile', [path, content]);
            return this._socket.sendBlocking(msg);
        }
    }, {
        key: 'onReady',
        value: function onReady(callback) {
            this._socket.onOpen(callback);
        }
    }, {
        key: 'on',
        value: function on(method, cb) {
            this._socket.on(method, cb);
        }
    }, {
        key: 'loadScript',
        value: function loadScript(path) {
            var _this = this;

            return this.absoluteFilePath(path).then(function (absolutePath) {
                return _this.readFile(absolutePath).then(function (content) {
                    var scriptElement = document.createElement('script');
                    scriptElement.setAttribute('type', 'text/javascript');
                    scriptElement.textContent = content;
                    return new Promise(function (resolve, reject) {
                        scriptElement.onload = resolve();
                        document.head.appendChild(scriptElement);
                        setTimeout(function () {
                            reject('Script loading timed out');
                        }, 5000);
                    });
                });
            });
        }
    }, {
        key: 'answered',
        value: function answered() {
            var msg = this._socket.buildInvokeMessage('answered', []);
            this._socket.sendBlocking(msg);
        }
    }, {
        key: '_onMessage',
        value: function _onMessage(msg) {}
    }, {
        key: '_loadHtml',
        value: function _loadHtml(data) {
            document.documentElement.outerHTML = data;
        }
    }, {
        key: '_evaluateJavaScript',
        value: function _evaluateJavaScript(script) {
            var result = eval(script);
            this.sendBlocking(this.buildInvokeMessage('javascriptFinished', [result]));
        }
    }]);

    return HtmlApi;
}();
