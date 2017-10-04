/*jshint esversion:6, curly: false, devel: true, browser: true */

let typeField = 'type';
let dataField = 'data';
let tokenField = 'token';
let methodField = 'method';
let classField = 'class';
let argumentsField = 'arguments';
let returnIdField = 'returnId';

let invokeType = 'invoke';
let returnType = 'return';
let userType = 'user';

class MessageValidator {
    isValid(message) {
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
}

class CommonSocket {
    constructor(url, token, messageHandler) {
        this._url = url;
        this._token = token;
        this._messageHandler = messageHandler;
        this._messageValidator = new MessageValidator();
        this._resolve = undefined;
        this._reject = undefined;
        this._promises = {};
        this._invokables = {};
    }

    open() {
        this._webSocket = new WebSocket(this._url);
        this._webSocket.onmessage = this._onMessage;
        this._webSocket.onclose = this._closeHandler;
        this._webSocket.parent = this;

    }

    close() {
        if (this._resolve !== undefined)
            this._reject('Closing socket');
        this._webSocket.onclose = undefined;
        this._webSocket.close();
    }

    buildUserMessage(data) {
        let message = {};
        message[tokenField] = this._token;
        message[typeField] = userType;
        message[dataField] = data;
        return message;
    }

    buildReturnMessage(data, uuid) {
        let message = {};
        message[tokenField] = this._token;
        message[typeField] = returnType;
        message[dataField] = data;
        message[returnIdField] = uuid;
        return message;
    }

    buildInvokeMessage(method, args) {
        let message = {};
        message[tokenField] = this._token;
        message[typeField] = invokeType;
        message[methodField] = method;
        message[returnIdField] = this._uuidv4();
        if (args.length !== 0)
            message[argumentsField] = args;
        return message;
    }

    on(method, invokable) {
        this._invokables = this._invokables || {};
        this._invokables[method] = this._invokables[method] || [];
        this._invokables[method].push(invokable);
    }

    removeListener(method, invokable) {
        this._invokables = this._invokables || {};
        this._invokables[method] = this._invokables[method].
            filter(e => e !== invokable);
    }

    removeAllListeners(method) {
        this._invokables = this._invokables || {};
        this._invokables[method] = [];
    }

    sendBlocking(message, timeout = 2000) {
        if (!this._messageValidator.isValid(message))
            throw new Error('Message ' + message + 'is invalid');

        return new Promise((resolve, reject) => {
            if (message[typeField] == invokeType) {
                let returnId = message[returnIdField];
                let promise = {
                    'resolve': resolve,
                    'reject': reject,
                };
                this._promises[returnId] = promise;
            }
            this._webSocket.send(JSON.stringify(message));
            setTimeout(() => {
                reject('Return promise timed out');
            }, timeout);
        });
    }

    sendAsync(message) {
        this._webSocket.send(JSON.stringify(message));
    }

    _onMessage(message) {
        let parsedMessage = JSON.parse(message.data);
        if (!this.parent._messageValidator.isValid(parsedMessage)) {
            console.error('Message received from server: ' +
                          parsedMessage + ' is not valid!');
            return;
        }

        if (parsedMessage.type === returnType &&
            this.parent._promises.hasOwnProperty(
                parsedMessage[returnIdField])) {
            this.parent._promises[parsedMessage[returnIdField]].resolve(
                parsedMessage.data);
            delete this.parent._promises[parsedMessage[returnIdField]];
        } else if (parsedMessage.type === invokeType) {
            let method = parsedMessage[methodField];
            let args = parsedMessage[argumentsField] || [];
            if (!this.parent._invokables.hasOwnProperty(method))
                console.error('Invokable ' + method + ' not in invokables');
            this.parent._invokables[method].forEach(invokable => {
                let returned = invokable.apply(this, args) ;
                this.parent.sendAsync(this.parent.buildReturnMessage(
                    returned, parsedMessage[returnIdField]));
            });
        } else {
            if (this.parent._messageHandler !== undefined)
                this.parent._messageHandler(parsedMessage);
        }
    }

    _closeHandler(closeEvent) {
        setTimeout(() => {
            let parent = this.parent;
            parent._webSocket = new WebSocket(parent._url);
            parent._webSocket.onmessage = parent._onMessage;
            parent._webSocket.onclose = parent._closeHandler;
            parent._webSocket.parent = parent;
            parent._resolve = undefined;
            parent._reject = undefined;
        }, 500);
    }

    _uuidv4() {
        return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
            var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
            return v.toString(16);
        });
    }
}
