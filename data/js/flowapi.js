'use strict';
/* eslint-disable no-unused-vars, no-undef */

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

var FlowApi = function () {
    function FlowApi(url, token) {
        _classCallCheck(this, FlowApi);

        this._url = url;
        this._token = token;
        this._socket = new CommonSocket(url, token, this._onMessage);
        this._socket.on('loadHtml', this._loadHtml);
        this._socket.on('evaluateJavaScript', this._evaluateJavaScript);
        this._socket.open();
    }

    _createClass(FlowApi, [{
        key: 'runExperiment',
        value: function runExperiment(path) {
            var _this = this;

            var expressions = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : {};
            var results = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : '';

            var promises = [];
            for (var key in expressions) {
                if (expressions.hasOwnProperty(key)) {
                    var expression = this._socket.buildInvokeMessage('addExpression', [key, expressions[key]]);
                    promises.push(this._socket.sendBlocking(expression));
                }
            }
            return Promise.all(promises).then(function () {
                return _this._socket.sendBlocking(_this._socket.buildInvokeMessage('runExperiment', [path, results]), 10000);
            });
        }
    }, {
        key: 'absoluteFilePath',
        value: function absoluteFilePath(path) {
            var msg = this._socket.buildInvokeMessage('absoluteFilePath', [path]);
            return this._socket.sendBlocking(msg);
        }
    }, {
        key: 'fromCurrentDir',
        value: function fromCurrentDir(path) {
            console.error('FromCurrentDir is deprecated, use absoluteFilePath instead');
            return this.absoluteFilePath(path);
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
            var _this2 = this;

            return this.absoluteFilePath(path).then(function (absolutePath) {
                return _this2.readFile(absolutePath).then(function (content) {
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
            eval(script);
        }
    }]);

    return FlowApi;
}();
