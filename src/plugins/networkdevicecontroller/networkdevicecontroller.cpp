/******************************************************************************
 * Copyright (C) 2016  Michael Hofmann <mh21@mh21.de>                         *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
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

#include "apexmain/device/plugincontrollerinterface.h"

#include "apextools/global.h"

#include <QEventLoop>
#include <QHostAddress>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(APEX_NWDC)
Q_LOGGING_CATEGORY(APEX_NWDC, "apex.networkdevicecontroller")

class NetworkDeviceControllerCreator : public QObject,
                                       public PluginControllerCreator
{
    Q_OBJECT
    Q_INTERFACES(PluginControllerCreator)

    Q_PLUGIN_METADATA(IID "apex.networkdevicecontroller")

public:
    virtual QStringList availablePlugins() const;

    virtual PluginControllerInterface *
    createController(const QString &name) const;
};

class NetworkDeviceController : public QObject, public PluginControllerInterface
{
    Q_OBJECT
public:
    NetworkDeviceController();
    ~NetworkDeviceController();

    virtual void resetParameters();

    virtual bool isValidParameter(const QString &type, int channel) const;
    virtual bool setParameter(const QString &type, int channel,
                              const QString &value);

    virtual bool prepare();
    virtual void playStimulus();
    virtual void release();

    bool send(const QString &message);
    bool waitForMessage(const QString &message);

private:
    void ensureConnection();

    QTcpSocket tcpSocket;

    QHostAddress host;
    unsigned port;
    int timeout;

    QString fileName;
    QString subject;
    QString note;

    QString startMessage;
    QString startMessageReply;
    QString stopMessage;
    QString stopMessageReply;

    bool startSent;
};

// NetworkDeviceController
// ===============================================================

NetworkDeviceController::NetworkDeviceController()
{
    resetParameters();
}

NetworkDeviceController::~NetworkDeviceController()
{
}

void NetworkDeviceController::resetParameters()
{
    timeout = 30000;
    startMessage = QL1S("Start|%1|%2|%3\r\n");
    startMessageReply = QL1S("Recording started\r\n");
    stopMessage = QL1S("Stop\r\n");
    stopMessageReply = QL1S("Recording stopped\r\n");
    startSent = false;
}

bool NetworkDeviceController::isValidParameter(const QString &type,
                                               int channel) const
{
    if (channel != -1)
        return false;
    if (type == QL1S("host"))
        return true;
    if (type == QL1S("port"))
        return true;
    if (type == QL1S("timeout"))
        return true;
    if (type == QL1S("filename"))
        return true;
    if (type == QL1S("subject"))
        return true;
    if (type == QL1S("note"))
        return true;
    if (type == QL1S("startmessage"))
        return true;
    if (type == QL1S("startmessagereply"))
        return true;
    if (type == QL1S("stopmessage"))
        return true;
    if (type == QL1S("stopmessagereply"))
        return true;
    return false;
}

bool NetworkDeviceController::setParameter(const QString &type, int channel,
                                           const QString &value)
{
    if (!isValidParameter(type, channel))
        return false;

    if (type == QL1S("host")) {
        host = QHostAddress(value);
        return true;
    }

    if (type == QL1S("port")) {
        bool ok;
        unsigned parsedValue = value.toUInt(&ok);
        if (!ok) {
            setErrorMessage(
                QString::fromLatin1("Unable to parse value %1 for parameter %2")
                    .arg(value)
                    .arg(type));
            return false;
        }
        port = parsedValue;
        return true;
    }

    if (type == QL1S("timeout")) {
        bool ok;
        int parsedValue = value.toInt(&ok);
        if (!ok) {
            setErrorMessage(
                QString::fromLatin1("Unable to parse value %1 for parameter %2")
                    .arg(value)
                    .arg(type));
            return false;
        }
        timeout = parsedValue;
        return true;
    }

    if (type == QL1S("filename")) {
        fileName = value;
        return true;
    }

    if (type == QL1S("subject")) {
        subject = value;
        return true;
    }

    if (type == QL1S("note")) {
        note = value;
        return true;
    }

    if (type == QL1S("startmessage")) {
        startMessage = value;
        return true;
    }

    if (type == QL1S("startmessagereply")) {
        startMessageReply = value;
        return true;
    }

    if (type == QL1S("stopmessage")) {
        stopMessage = value;
        return true;
    }

    if (type == QL1S("stopmessagereply")) {
        stopMessageReply = value;
        return true;
    }

    return true;
}

bool NetworkDeviceController::prepare()
{
    /* http://doc.qt.io/qt-5/qabstractsocket.html#waitForConnected
     * QAbstractSocket::waitForConnected may fail randomly on Windows, so a
     * little eventloop is needed.
     */

    if (tcpSocket.state() == QAbstractSocket::ConnectedState)
        return true;

    if (!startSent) {
        startSent = true;
        bool acknowledged = false;
        do {
            if (send(startMessage.arg(fileName).arg(subject).arg(note)))
                acknowledged = waitForMessage(startMessageReply);

            if (!acknowledged) {
                QMessageBox::StandardButton answer = QMessageBox::question(
                    nullptr, QSL("Start failed"),
                    QString::fromLatin1(
                        "Retry starting actiview at %1 on port %2?")
                        .arg(host.toString())
                        .arg(port));
                if (answer != QMessageBox::StandardButton::Yes)
                    return false;
            }
        } while (!acknowledged);
    }
    return true;
}

void NetworkDeviceController::playStimulus()
{
}

void NetworkDeviceController::release()
{
    send(stopMessage);
    waitForMessage(stopMessageReply);
    tcpSocket.disconnectFromHost();
}

bool NetworkDeviceController::send(const QString &message)
{
    ensureConnection();
    if (!tcpSocket.isValid()) {
        qCWarning(APEX_NWDC, "Socket not valid, unable to send message: \"%s\"",
                  qPrintable(message.trimmed()));
        return false;
    }
    qCDebug(APEX_NWDC, "Sending message: \"%s\"",
            qPrintable(message.trimmed()));
    return tcpSocket.write(message.toUtf8()) != -1;
}

bool NetworkDeviceController::waitForMessage(const QString &message)
{
    ensureConnection();
    if (!tcpSocket.canReadLine()) {
        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop loop;
        connect(&tcpSocket, SIGNAL(readyRead()), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.start(timeout);
        loop.exec();
        disconnect(&tcpSocket, SIGNAL(readyRead()), &loop, SLOT(quit()));
    }

    setErrorMessage(QString());
    if (tcpSocket.canReadLine()) {
        Q_FOREACH (const QString &line,
                   message.split(QString::fromLatin1("\r\n"))) {
            QString readLine = QString::fromUtf8(tcpSocket.readLine());
            qCDebug(APEX_NWDC, "Received message: \"%s\"",
                    qPrintable(readLine.trimmed()));
            if (readLine.trimmed() != line) {
                QString error =
                    QString::fromLatin1(
                        "Line \"%1\" is not equal to expected \"%2\"")
                        .arg(readLine.trimmed())
                        .arg(line.trimmed());
                setErrorMessage(error);
                qCWarning(APEX_NWDC, "Error receiving message: %s",
                          qPrintable(error));
                return false;
            }
        }
        qCDebug(APEX_NWDC, "Server acknowledged with message: \"%s\"",
                qPrintable(message.trimmed()));
        return true;
    } else {
        QString error =
            QString::fromLatin1("Timeout waiting for message \"%1\"")
                .arg(message.trimmed());
        setErrorMessage(error);
        qCWarning(APEX_NWDC, "%s", qPrintable(error));
        return false;
    }
}

void NetworkDeviceController::ensureConnection()
{
    while (tcpSocket.state() != QAbstractSocket::ConnectedState) {
        QTimer timer;
        timer.setSingleShot(true);
        QEventLoop loop;
        connect(&tcpSocket, SIGNAL(connected()), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        tcpSocket.connectToHost(host, port);
        timer.start(timeout);
        loop.exec();

        if (!timer.isActive()) {
            QMessageBox::StandardButton answer = QMessageBox::question(
                nullptr, QSL("Connection failed"),
                QSL("Establishing Apex-Labview connection failed, try again?"));
            if (answer != QMessageBox::StandardButton::Yes) {
                QString error = QString::fromLatin1("Unable to connect "
                                                    "networkdevicecontroller "
                                                    "to host %1 on port %2")
                                    .arg(host.toString())
                                    .arg(port);
                setErrorMessage(error);
                qCWarning(APEX_NWDC, "%s", qPrintable(error));
            }
        }
    }
}

// NetworkDeviceControllerCreator
// ========================================================

QStringList NetworkDeviceControllerCreator::availablePlugins() const
{
    return QStringList() << QL1S("networkdevicecontroller");
}

PluginControllerInterface *
NetworkDeviceControllerCreator::createController(const QString &name) const
{
    try {
        if (name == QL1S("networkdevicecontroller"))
            return new NetworkDeviceController();
    } catch (...) {
    }

    return nullptr;
}

#include "networkdevicecontroller.moc"
