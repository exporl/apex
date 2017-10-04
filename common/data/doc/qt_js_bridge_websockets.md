### WebSocketServer

The websocket server handles all communication between apex and the webview.

#### Message types

Json is used in all communication. A message always has a type field, and
depending on the type the server will look for other fields.

The server exposes some useful static methods to build messages.
It can also hold a welcome message which will be sent to the client
when it connects.

##### Invoke message

```
{
type: "invoke",
method: "myMethod,
arguments: ["myArgument", "myOtherArgument", ...],
returnId: "someUniqueId"
}
```

This message is meant for calling exposed apex methods from the webview.
The websocket server allows installing QObjects of which Q_INVOKABLES can be
invoked with this message.

The arguments field can be omitted if the method requires no arguments.

Note that this invoke is asynchronous. The javascript wrapper allows for a
promise-based synchronous invoke.

The returnId is used internally to map a return message to an invoke message.

##### Return message

```
{
type: "return",
data: "myReturnData",
returnId: "someUniqueId"
}
```

This message contains the result of an invoke message. Note that invoking
a method with a void return type will also send a return message, but
without data. The javascript client wrapper uses this to determine
when it should resolve the promise when using a synchronous invoke.

The returnId is used internally to map a return message to an invoke message.
The websocket client for instance uses it to keep track of each promise
tied to a blocking invoke message. Once it receives a return message for that
invoke message, it knows which promise to resolve.

##### User message

```
{
type: "user",
data: "<p>Hello World!</p>"
}
```

This message is meant for any sort of custom message. As a result this message
is not handled by the server or the javascript client wrapper itself and should be
handled by the user.

#### CSRF token

```
{
...
token: "base64token"
}
```

A token is generated when the webserver is constructed.
Every websocket client should include this token in each message that it sends.
If a client fails to include this token the server will disconnected the client.

This token is queryable by WebSocketServer::csrfToken.
Note that this token should not be sent by the websocket server,
so do not include this token in the welcome message.
