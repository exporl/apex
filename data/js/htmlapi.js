'use strict';

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
        key: 'on',
        value: function on(method, cb) {
            this._socket.on(method, cb);
        }
    }, {
        key: 'answered',
        value: function answered() {
            var msg = this._socket.buildInvokeMessage('answered', []);
            return this._socket.sendBlocking(msg);
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

    return HtmlApi;
}();
