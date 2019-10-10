# Flowrunner

## Initialize

To initialize communication with the flow API a WebSocket needs to be created, using the injected server port. The WebSocket is needed to create a QWebChannel that facilitates communication with Apex. The QWebChannel is available through the qwebchannel.js javascript file. This file will automatically be made available by Apex when the flow is started.

Import the QWebChannel code with this script-tag:

```html
<script src="qwebchannel.js"></script>
```

Initialize the WebSocket and the QWebChannel with this javascript fragment. The comment indicates a place where an initialized api is available.

```javascript
function whenInitialized(callback) {
    window.onload = function() {
        var socket = new WebSocket("ws://127.0.0.1:{serverPort}");

        socket.onopen = function() {
            new QWebChannel(socket, function(channel) {
                callback(channel.objects.flowApi);
            });
        };
    };
}

whenInitialized(function(api) {
    // initialized api available here
});
```

## Call API method

API methods are easily called with a callback. All calls are asynchronous.

```javascript
api.readFile("path/to/file.apr", function(fileContent) {
    // fileContent available
});
```

## Connect to API signal

It's possible to listen for emitted signals on the API.

```javascript
api.experimentDone.connect(function(resultfilePath) {
    // resultfilePath available
});
```

## Disconnect from signal

Sometimes you also want to disconnect from a signal (after it's emitted the first time for example). You can disconnect the same way you connected, supplying the callback you used to connect.

```javascript
function callback(resultfilePath) {
    api.experimentDone.disconnect(callback);
    resolve(resultfilePath);
}
api.experimentDone.connect(callback);
```

## Slides

Attachment: [Slides of Flowrunner workshop (25/04/2018)](2018-04-25_FlowrunnerInfo_slideshow.pdf)