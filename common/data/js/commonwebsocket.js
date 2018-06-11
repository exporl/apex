'use strict';

var _createClass = function () { function defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } } return function (Constructor, protoProps, staticProps) { if (protoProps) defineProperties(Constructor.prototype, protoProps); if (staticProps) defineProperties(Constructor, staticProps); return Constructor; }; }();

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

var typeField = 'type';
var dataField = 'data';
var tokenField = 'token';
var methodField = 'method';
var classField = 'class';
var argumentsField = 'arguments';
var returnIdField = 'returnId';

var invokeType = 'invoke';
var returnType = 'return';
var userType = 'user';

var MessageValidator = function () {
    function MessageValidator() {
        _classCallCheck(this, MessageValidator);
    }

    _createClass(MessageValidator, [{
        key: 'isValid',
        value: function isValid(message) {
            if (!message.hasOwnProperty(typeField)) {
                console.error('No type field present in message: ' + message);
                return false;
            }
            switch (message.type) {
                case invokeType:
                    if (!message.hasOwnProperty(methodField)) {
                        console.error('Invoke message doesn\'t name method to invoke');
                        return false;
                    }
                    if (!message.hasOwnProperty(returnIdField)) {
                        console.error('Invoke message doesn\'t include returnId field');
                        return false;
                    }
                    break;
                case userType:
                    if (!message.hasOwnProperty(dataField)) {
                        console.error('Content message doesn\'t include data field');
                        return false;
                    }
                    break;
                case returnType:
                    if (!message.hasOwnProperty(returnIdField)) {
                        console.error('Return message doesn\'t include returnId field');
                        return false;
                    }
                    break;
                default:
                    console.error('Unknown type ' + message.type);
            }
            return true;
        }
    }]);

    return MessageValidator;
}();

var CommonSocket = function () {
    function CommonSocket(url, token, messageHandler) {
        _classCallCheck(this, CommonSocket);

        this._url = url;
        this._token = token;
        this._messageHandler = messageHandler;
        this._messageValidator = new MessageValidator();
        this._resolve = undefined;
        this._reject = undefined;
        this._promises = {};
        this._invokables = {};
    }

    _createClass(CommonSocket, [{
        key: 'open',
        value: function open() {
            this._webSocket = new WebSocket(this._url);
            this._webSocket.onmessage = this._onMessage;
            this._webSocket.onclose = this._closeHandler;
            this._webSocket.parent = this;
        }
    }, {
        key: 'close',
        value: function close() {
            if (this._resolve !== undefined) this._reject('Closing socket');
            this._webSocket.onclose = undefined;
            this._webSocket.close();
        }
    }, {
        key: 'onOpen',
        value: function onOpen(callback) {
            if (this._webSocket.readyState === WebSocket.OPEN) callback();
            this._webSocket.onopen = callback;
        }
    }, {
        key: 'buildUserMessage',
        value: function buildUserMessage(data) {
            var message = {};
            message[tokenField] = this._token;
            message[typeField] = userType;
            message[dataField] = data;
            return message;
        }
    }, {
        key: 'buildReturnMessage',
        value: function buildReturnMessage(data, uuid) {
            var message = {};
            message[tokenField] = this._token;
            message[typeField] = returnType;
            message[dataField] = data;
            message[returnIdField] = uuid;
            return message;
        }
    }, {
        key: 'buildInvokeMessage',
        value: function buildInvokeMessage(method, args) {
            var message = {};
            message[tokenField] = this._token;
            message[typeField] = invokeType;
            message[methodField] = method;
            message[returnIdField] = this._uuidv4();
            if (args.length !== 0) message[argumentsField] = args;
            return message;
        }
    }, {
        key: 'on',
        value: function on(method, invokable) {
            this._invokables = this._invokables || {};
            this._invokables[method] = this._invokables[method] || [];
            this._invokables[method].push(invokable);
        }
    }, {
        key: 'removeListener',
        value: function removeListener(method, invokable) {
            this._invokables = this._invokables || {};
            this._invokables[method] = this._invokables[method].filter(function (e) {
                return e !== invokable;
            });
        }
    }, {
        key: 'removeAllListeners',
        value: function removeAllListeners(method) {
            this._invokables = this._invokables || {};
            this._invokables[method] = [];
        }
    }, {
        key: 'sendBlocking',
        value: function sendBlocking(message) {
            var _this = this;

            var timeout = arguments.length > 1 && arguments[1] !== undefined ? arguments[1] : 2000;

            if (!this._messageValidator.isValid(message)) throw new Error('Message ' + message + 'is invalid');

            return new Promise(function (resolve, reject) {
                if (message[typeField] == invokeType) {
                    var returnId = message[returnIdField];
                    var promise = {
                        'resolve': resolve,
                        'reject': reject
                    };
                    _this._promises[returnId] = promise;
                }
                _this._webSocket.send(JSON.stringify(message));
                setTimeout(function () {
                    reject('Return promise timed out');
                }, timeout);
            });
        }
    }, {
        key: 'sendAsync',
        value: function sendAsync(message) {
            this._webSocket.send(JSON.stringify(message));
        }
    }, {
        key: '_onMessage',
        value: function _onMessage(message) {
            var _this2 = this;

            var parsedMessage = JSON.parse(message.data);
            if (!this.parent._messageValidator.isValid(parsedMessage)) {
                console.error('Message received from server: ' + parsedMessage + ' is not valid!');
                return;
            }

            if (parsedMessage.type === returnType && this.parent._promises.hasOwnProperty(parsedMessage[returnIdField])) {
                this.parent._promises[parsedMessage[returnIdField]].resolve(parsedMessage.data);
                delete this.parent._promises[parsedMessage[returnIdField]];
            } else if (parsedMessage.type === invokeType) {
                var method = parsedMessage[methodField];
                var args = parsedMessage[argumentsField] || [];
                if (!this.parent._invokables.hasOwnProperty(method)) console.error('Invokable ' + method + ' not in invokables');
                this.parent._invokables[method].forEach(function (invokable) {
                    var returned = invokable.apply(_this2.parent, args);
                    _this2.parent.sendAsync(_this2.parent.buildReturnMessage(returned, parsedMessage[returnIdField]));
                });
            } else {
                if (this.parent._messageHandler !== undefined) this.parent._messageHandler(parsedMessage);
            }
        }
    }, {
        key: '_closeHandler',
        value: function _closeHandler(closeEvent) {
            var _this3 = this;

            setTimeout(function () {
                var parent = _this3.parent;
                parent._webSocket = new WebSocket(parent._url);
                parent._webSocket.onmessage = parent._onMessage;
                parent._webSocket.onclose = parent._closeHandler;
                parent._webSocket.parent = parent;
                parent._resolve = undefined;
                parent._reject = undefined;
            }, 500);
        }
    }, {
        key: '_uuidv4',
        value: function _uuidv4() {
            return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
                var r = Math.random() * 16 | 0,
                    v = c == 'x' ? r : r & 0x3 | 0x8;
                return v.toString(16);
            });
        }
    }]);

    return CommonSocket;
}();
