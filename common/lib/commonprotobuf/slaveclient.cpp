/******************************************************************************
 * Copyright (C) 2008-2013  Michael Hofmann <mh21@mh21.de>                    *
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

#include "slaveclient.h"
#include "protoslave.pb.h"
#include "common/exception.h"
#include "common/utils.h"

#include <QLocalSocket>

#include <QtCore>

namespace cmn
{

class SlaveClientPrivate : public QObject
{
    Q_OBJECT
public:
    void reply(const QByteArray &result,
               const QByteArray &error = QByteArray());

public Q_SLOTS:
    void readyRead();

public:
    QObject *commands;
    QLocalSocket socket;
    QByteArray data;
};

// SlaveClientPrivate ==========================================================

void SlaveClientPrivate::readyRead()
{
    data += socket.readAll();
    try {
        Q_FOREVER {
            if (data.size() < 4)
                return;

            const quint32 size(qFromLittleEndian<quint32>(
                reinterpret_cast<const unsigned char *>(data.constData())));
            if (size > INT_MAX)
                throw Exception(tr("Command packet is too big"));

            if (unsigned(data.size()) < size + 4)
                return;

            ProtoSlaveRequest request;
            if (!request.ParseFromArray(data.constData() + 4, size))
                throw Exception(tr("Unable to parse datagram"));

            data = data.mid(size + 4);

            //            qCDebug(EXPORL_COMMON, "Received request datagram:");
            //            qCDebug(EXPORL_COMMON, "  Name: %s",
            //            request.function().c_str());
            //            qCDebug(EXPORL_COMMON, "  Parameters: %i",
            //            request.parameter_size());
            //            Q_FOREACH (const auto &parameter, request.parameter())
            //                qCDebug(EXPORL_COMMON, "    Parameter: %s",
            //                parameter.c_str());
            //            qCDebug(EXPORL_COMMON, "  Output: %i",
            //            request.output());

            QByteArray resultValue;
            if (!QMetaObject::invokeMethod(
                    commands, request.function().c_str(),
                    request.output() ? Q_RETURN_ARG(QByteArray, resultValue)
                                     : QGenericReturnArgument(),
                    request.parameter_size() > 0
                        ? Q_ARG(QByteArray,
                                stdStringToRawByteArray(request.parameter(0)))
                        : QGenericArgument(),
                    request.parameter_size() > 1
                        ? Q_ARG(QByteArray,
                                stdStringToRawByteArray(request.parameter(1)))
                        : QGenericArgument(),
                    request.parameter_size() > 2
                        ? Q_ARG(QByteArray,
                                stdStringToRawByteArray(request.parameter(2)))
                        : QGenericArgument(),
                    request.parameter_size() > 3
                        ? Q_ARG(QByteArray,
                                stdStringToRawByteArray(request.parameter(3)))
                        : QGenericArgument())) {
                throw Exception(
                    tr("Unable to call slot %1")
                        .arg(QString::fromStdString(request.function())));
            }
            reply(resultValue);
        }
    } catch (const std::exception &e) {
        data.clear();
        reply(QByteArray(), e.what());
    }
}

void SlaveClientPrivate::reply(const QByteArray &result,
                               const QByteArray &error)
{
    ProtoSlaveReply reply;
    if (!result.isEmpty())
        reply.set_result(result.constData(), result.size());
    if (!error.isEmpty())
        reply.set_error(error.constData(), error.size());

    QByteArray datagram(reply.ByteSize() + 4, 0);
    qToLittleEndian<quint32>(
        reply.GetCachedSize(),
        reinterpret_cast<unsigned char *>(datagram.data()));
    reply.SerializeWithCachedSizesToArray(
        reinterpret_cast<unsigned char *>(datagram.data()) + 4);

    const int written = socket.write(datagram);
    Q_UNUSED(written);
    Q_ASSERT(written == datagram.size());
}

// SlaveClient =================================================================

SlaveClient::SlaveClient(QObject *commands, const QString &serverName)
    : dataPtr(new SlaveClientPrivate)
{
    E_D(SlaveClient);

    d->commands = commands;

    connect(&d->socket, SIGNAL(readyRead()), d, SLOT(readyRead()));
    connect(&d->socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    d->socket.connectToServer(serverName);
}

SlaveClient::~SlaveClient()
{
    delete dataPtr;
}
}

#include "slaveclient.moc"
