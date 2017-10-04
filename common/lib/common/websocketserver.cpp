/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "websocketserver.h"

#include "exception.h"
#include "random.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaMethod>
#include <QPointer>
#include <QUuid>
#include <QWebSocket>
#include <QWebSocketServer>

namespace cmn
{

namespace
{
static const QString typeField = QSL("type");
static const QString dataField = QSL("data");
static const QString tokenField = QSL("token");
static const QString classField = QSL("class");
static const QString methodField = QSL("method");
static const QString argumentsField = QSL("arguments");
static const QString returnIdField = QSL("returnId");

static const QString invokeType = QSL("invoke");
static const QString returnType = QSL("return");
static const QString userType = QSL("user");

typedef QPair<QPointer<QObject>, QString> InvokableObject;
}

class WebSocketServerPrivate : public QObject
{
    Q_OBJECT

public:
    void broadcastMessage(const QJsonObject &message);
    void invokeMethod(const QJsonObject &method);

public Q_SLOTS:
    void onNewConnection();
    void onSocketDisconnected();
    void onMessageReceived(const QString &message);

Q_SIGNALS:
    void messageReceived(const QJsonObject &message);

public:
    QWebSocketServer *webSocketServer;
    QByteArray csrfToken;
    QList<QWebSocket *> webSockets;
    QMap<QString, QList<InvokableObject>> invokables;
};

void WebSocketServerPrivate::onNewConnection()
{
    QWebSocket *newSocket = webSocketServer->nextPendingConnection();
    webSockets.append(newSocket);
    connect(newSocket, SIGNAL(textMessageReceived(QString)), this,
            SLOT(onMessageReceived(QString)));
    connect(newSocket, SIGNAL(disconnected()), this,
            SLOT(onSocketDisconnected()));
}

void WebSocketServerPrivate::onSocketDisconnected()
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    webSockets.removeAll(socket);
    socket->deleteLater();
}

void WebSocketServerPrivate::onMessageReceived(const QString &message)
{
    QJsonDocument document = QJsonDocument::fromJson(message.toUtf8());
    if (!document.isObject())
        qCWarning(EXPORL_COMMON, "Message %s is not a JSON Object",
                  qPrintable(message));

    QJsonObject jsonObject = document.object();
    if (!jsonObject.contains(tokenField) ||
        jsonObject.value(tokenField).toString().toUtf8() != csrfToken) {
        qCWarning(EXPORL_COMMON, "Token not present or not matching %s",
                  qPrintable(message));
        qobject_cast<QWebSocket *>(sender())->close();
    }

    const QString messageType = jsonObject.value(typeField).toString();
    if (messageType == invokeType)
        invokeMethod(jsonObject);
    else if (messageType == userType || messageType == returnType)
        Q_EMIT messageReceived(jsonObject);
    else
        qCWarning(EXPORL_COMMON, "Unkown type %s. Message was %s ignored.",
                  qPrintable(messageType), qPrintable(message));
}

void WebSocketServerPrivate::invokeMethod(const QJsonObject &method)
{
    QString methodName = method.value(methodField).toString().remove(QSL(" "));
    QString className = method.value(classField).toString();

    const QList<InvokableObject> invokableObjects =
        invokables.value(methodName);
    RETURN_IF_FAIL(!invokableObjects.isEmpty());
    RETURN_IF_FAIL(method.contains(returnIdField));
    QJsonArray jsonArgs = method.value(argumentsField).toArray();

    Q_FOREACH (const InvokableObject &invokableObject, invokableObjects) {
        /* continue if the object is null. Note that there is a small chance of
         * failure if the object gets deleted between this check
         * and the retrieving of the metamethod. Remove qobjects
         * from the invokables before deleting them!
         */
        if (invokableObject.first.isNull())
            continue;

        const QMetaObject *metaObject = invokableObject.first->metaObject();
        QMetaMethod metaMethod = metaObject->method(
            metaObject->indexOfMethod(invokableObject.second.toUtf8()));
        if (!metaMethod.isValid()) {
            qCWarning(EXPORL_COMMON, "Method signature invalid %s",
                      qPrintable(methodName));
            continue;
        }

        QList<QGenericArgument> arguments;
        QList<QByteArray> parameterTypes = metaMethod.parameterTypes();
        /* We use these to prevent the arguments from going out of scope */
        QList<int> intArgs;
        QList<double> doubleArgs;
        QList<bool> boolArgs;
        QList<QString> stringArgs;
        QList<QVariant> variantArgs;
        Q_FOREACH (const QJsonValue &jsonVal, jsonArgs) {
            int parameterType = QMetaType::type(parameterTypes.takeFirst());
            switch (parameterType) {
            case QMetaType::Int:
                intArgs.append(jsonVal.toInt());
                arguments.append(Q_ARG(int, intArgs.last()));
                break;
            case QMetaType::Double:
                doubleArgs.append(jsonVal.toDouble());
                arguments.append(Q_ARG(double, doubleArgs.last()));
                break;
            case QMetaType::Bool:
                boolArgs.append(jsonVal.toBool());
                arguments.append(Q_ARG(bool, boolArgs.last()));
                break;
            case QMetaType::QString:
                stringArgs.append(jsonVal.toString());
                arguments.append(Q_ARG(QString, stringArgs.last()));
                break;
            case QMetaType::QVariant:
                variantArgs.append(jsonVal.toVariant());
                arguments.append(Q_ARG(QVariant, variantArgs.last()));
                break;
            default:
                qCCritical(EXPORL_COMMON, "Argument of type %s not supported",
                           QMetaType::typeName(parameterType));
                continue;
            }
        }

        int intResult;
        double doubleResult;
        bool boolResult;
        QString stringResult;
        QVariant variantResult;
        QGenericReturnArgument returnArgument;
        switch (metaMethod.returnType()) {
        case QMetaType::Void:
            returnArgument = QGenericReturnArgument();
            break;
        case QMetaType::Int:
            returnArgument = Q_RETURN_ARG(int, intResult);
            break;
        case QMetaType::Double:
            returnArgument = Q_RETURN_ARG(double, doubleResult);
            break;
        case QMetaType::Bool:
            returnArgument = Q_RETURN_ARG(bool, boolResult);
            break;
        case QMetaType::QString:
            returnArgument = Q_RETURN_ARG(QString, stringResult);
            break;
        case QMetaType::QVariant:
            returnArgument = Q_RETURN_ARG(QVariant, variantResult);
            break;
        default:
            qCCritical(EXPORL_COMMON,
                       "Return argument of type %s not supported",
                       QMetaType::typeName(metaMethod.returnType()));
            continue;
        }

        QMetaObject::invokeMethod(
            invokableObject.first.data(), metaMethod.name(),
            Qt::DirectConnection, returnArgument,
            arguments.size() > 0 ? arguments.at(0) : QGenericArgument(),
            arguments.size() > 1 ? arguments.at(1) : QGenericArgument(),
            arguments.size() > 2 ? arguments.at(2) : QGenericArgument(),
            arguments.size() > 3 ? arguments.at(3) : QGenericArgument(),
            arguments.size() > 4 ? arguments.at(4) : QGenericArgument(),
            arguments.size() > 5 ? arguments.at(5) : QGenericArgument(),
            arguments.size() > 6 ? arguments.at(6) : QGenericArgument(),
            arguments.size() > 7 ? arguments.at(7) : QGenericArgument(),
            arguments.size() > 8 ? arguments.at(8) : QGenericArgument(),
            arguments.size() > 9 ? arguments.at(9) : QGenericArgument());

        QJsonObject returnObject;
        returnObject.insert(returnIdField,
                            QJsonValue(method.value(returnIdField)));
        switch (metaMethod.returnType()) {
        case QMetaType::Int:
            returnObject.insert(dataField, QJsonValue(intResult));
            break;
        case QMetaType::Double:
            returnObject.insert(dataField, QJsonValue(doubleResult));
            break;
        case QMetaType::Bool:
            returnObject.insert(dataField, QJsonValue(boolResult));
            break;
        case QMetaType::QString:
            returnObject.insert(dataField, QJsonValue(stringResult));
            break;
        case QMetaType::QVariant:
            returnObject.insert(dataField,
                                QJsonValue(variantResult.toString()));
            break;
        case QMetaType::Void:
            returnObject.insert(dataField, QJsonValue());
            break;
        }
        returnObject.insert(typeField, QJsonValue(returnType));
        broadcastMessage(returnObject);
    }
}

void WebSocketServerPrivate::broadcastMessage(const QJsonObject &message)
{
    const QString stringified =
        QString::fromUtf8(QJsonDocument(message).toJson());

    Q_FOREACH (QWebSocket *webSocket, webSockets) {
        webSocket->sendTextMessage(stringified);
        webSocket->flush();
    }
}

// WebSocketServer =============================================================

WebSocketServer::WebSocketServer(const QString &serverName)
    : dataPtr(new WebSocketServerPrivate)
{
    E_D(WebSocketServer);

    d->webSocketServer =
        new QWebSocketServer(serverName, QWebSocketServer::NonSecureMode, d);

    Random random;
    d->csrfToken.resize(32);
    random.nextBytes(d->csrfToken.data(), 32);
    d->csrfToken = d->csrfToken.toBase64();

    connect(d->webSocketServer, SIGNAL(newConnection()), d,
            SLOT(onNewConnection()));
    connect(d->webSocketServer, SIGNAL(newConnection()), this,
            SIGNAL(newConnection()));
    connect(d, SIGNAL(messageReceived(const QJsonObject &)), this,
            SIGNAL(messageReceived(const QJsonObject &)));
}

WebSocketServer::~WebSocketServer()
{
    E_D(WebSocketServer);
    qDeleteAll(d->webSockets.begin(), d->webSockets.end());
    delete dataPtr;
}

void WebSocketServer::on(const QString &method, QObject *object,
                         const QString &slot)
{
    E_D(WebSocketServer);
    d->invokables[QString(method).remove(QSL(" "))].append(
        qMakePair<QPointer<QObject>, QString>(QPointer<QObject>(object), slot));
}

void WebSocketServer::removeListener(const QString &method, QObject *object,
                                     const QString &slot)
{
    E_D(WebSocketServer);
    d->invokables[method].removeAll(
        qMakePair<QPointer<QObject>, QString>(QPointer<QObject>(object), slot));
}

void WebSocketServer::removeAllListeners(const QString &method)
{
    E_D(WebSocketServer);
    d->invokables.remove(method);
}

void WebSocketServer::start(const QHostAddress &address, quint16 port)
{
    E_D(WebSocketServer);

    if (d->webSocketServer->isListening())
        throw Exception(tr("Server is already listening"));

    if (!d->webSocketServer->listen(address, port))
        throw Exception(tr("Couldn't start listening at %s:%d")
                            .arg(d->webSocketServer->serverAddress().toString())
                            .arg(d->webSocketServer->serverPort()));
}

void WebSocketServer::stop()
{
    E_D(WebSocketServer);

    if (d->webSocketServer->isListening())
        d->webSocketServer->close();

    Q_FOREACH (QWebSocket *webSocket, d->webSockets) {
        webSocket->close();
        webSocket->deleteLater();
    }
    d->webSockets.clear();
}

quint16 WebSocketServer::serverPort()
{
    E_D(WebSocketServer);
    return d->webSocketServer->serverPort();
}

QString WebSocketServer::csrfToken()
{
    E_D(WebSocketServer);
    return QString::fromUtf8(d->csrfToken);
}

void WebSocketServer::broadcastMessage(const QJsonObject &jsonObject)
{
    E_D(WebSocketServer);
    d->broadcastMessage(jsonObject);
}

QJsonObject WebSocketServer::buildUserMessage(const QString &message)
{
    QJsonObject jsonObject;
    jsonObject.insert(typeField, QJsonValue(userType));
    jsonObject.insert(dataField, QJsonValue(message));
    return jsonObject;
}

QJsonObject WebSocketServer::buildInvokeMessage(const QString &method,
                                                const QVariantList &arguments)
{
    QJsonObject jsonObject;
    jsonObject.insert(typeField, QJsonValue(invokeType));
    jsonObject.insert(methodField, QJsonValue(method));
    jsonObject.insert(argumentsField,
                      QJsonValue(QJsonArray::fromVariantList(arguments)));
    jsonObject.insert(returnIdField,
                      QJsonValue(QUuid::createUuid().toString()));
    return jsonObject;
}
}

#include "websocketserver.moc"
