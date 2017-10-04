/******************************************************************************
 * Copyright (C) 2016  Michael Hofmann <mh21@mh21.de>                         *
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

#include "slaveserver.h"
#include "exception.h"
#include "paths.h"
#include "protoslave.pb.h"
#include "random.h"
#include "utils.h"

#include <QDir>
#include <QLocalServer>
#include <QLocalSocket>
#include <QProcess>
#include <QSettings>

#include <QtEndian>

namespace cmn
{

Q_GLOBAL_STATIC(Random, slaveServerId)

class SlaveServerPrivate
{
public:
    QString pythonPath();
    void handleResponse(QByteArray *output);

public:
    QString programName;
    QString arguments;
    QString pythonVersion;
    bool verbose;
    QString programPath;
    QLocalSocket *socket;
    QProcess process;
    QScopedPointer<QLocalServer> server;
};

// SlaveServerPrivate ==========================================================

QString SlaveServerPrivate::pythonPath()
{
    QString path(
        QString::fromLatin1("Software/Python/PythonCore/%1/InstallPath/Default")
            .arg(pythonVersion));
    QSettings userSettings(QL1S("HKEY_CURRENT_USER"), QSettings::NativeFormat);
    QSettings systemSettings(QL1S("HKEY_LOCAL_MACHINE"),
                             QSettings::NativeFormat);
    QString user(userSettings.value(path).toString());
    QString system(systemSettings.value(path).toString());
    return (!user.isEmpty() ? user : system) + QL1S("python.exe");
}

void SlaveServerPrivate::handleResponse(QByteArray *output)
{
    QByteArray data;
    Q_FOREVER {
        // sending stop might actually kill the slave directly without sending a
        // reply
        if (socket->state() != QLocalSocket::ConnectedState)
            return;

        socket->waitForReadyRead();
        data += socket->readAll();

        if (data.size() < 4)
            continue;
        const quint32 size(qFromLittleEndian<quint32>(
            reinterpret_cast<unsigned char *>(data.data())));
        if (size > INT_MAX)
            throw Exception(SlaveServer::tr("Reply packet is too big"));

        if (unsigned(data.size()) < size + 4)
            continue;

        ProtoSlaveReply reply;
        if (!reply.ParseFromArray(data.constData() + 4, size))
            throw Exception(SlaveServer::tr("Unable to parse datagram"));

        if (verbose) {
            qCDebug(EXPORL_COMMON, "Received reply datagram:");
            qCDebug(EXPORL_COMMON, "  Result: %s", reply.result().c_str());
            qCDebug(EXPORL_COMMON, "  Error: %s", reply.error().c_str());
        }

        if (reply.has_error())
            throw Exception(
                QString::fromUtf8(stdStringToRawByteArray(reply.error())));

        if (reply.has_result() != (output != NULL))
            throw Exception(SlaveServer::tr(
                "Reply packet contained wrong number of output parameters"));

        if (output)
            *output = stdStringToByteArray(reply.result());

        return;
    }
}

// SlaveServer =================================================================

SlaveServer::SlaveServer(const QString &programName, const QString &arguments,
                         const QString &pythonVersion, bool verbose)
    : dataPtr(new SlaveServerPrivate)
{
    E_D(SlaveServer);

    d->programName = programName;
    d->arguments = arguments;
    d->pythonVersion = pythonVersion.isEmpty() ? QSL("2.5") : pythonVersion;
    d->verbose = verbose;
    d->socket = NULL;

    const QStringList programs = QStringList() << programName + QL1S(".py")
#ifdef Q_OS_WIN32
                                               << programName + QL1S(".exe");
#else
                                               << programName;
#endif
    QStringList binDirectories(Paths::binDirectories());
    Q_FOREACH (const QString &program, programs) {
        d->programPath = Paths::searchFile(program, binDirectories);
        if (!d->programPath.isEmpty())
            break;
    }
    if (d->programPath.isEmpty())
        throw Exception(tr("Unable to find slave helper %1 while looking in %2")
                            .arg(programName, binDirectories.join(QL1S(", "))));

    connect();
}

SlaveServer::~SlaveServer()
{
    stop();
    delete dataPtr;
}

void SlaveServer::connect()
{
    E_D(SlaveServer);

    if (d->verbose)
        qCDebug(EXPORL_COMMON, "Starting helper program: %s",
                qPrintable(d->programPath));
    stop();

    // mix application pid as the random generator might be in deterministic
    // mode, which would always result in the same server names
    const QString serverName = QString::fromLatin1("proto-slave-%1")
                                   .arg(slaveServerId()->nextULongLong() ^
                                        QCoreApplication::applicationPid());

    d->server.reset(new QLocalServer());
    d->server->listen(serverName);

    d->process.setProcessChannelMode(d->verbose ? QProcess::ForwardedChannels
                                                : QProcess::MergedChannels);

#ifdef Q_OS_WIN32
    if (d->programPath.endsWith(QL1S(".py")))
        d->process.start(
            d->pythonPath(),
            QStringList(d->programPath)
                << serverName
                << d->arguments.split(QL1C(' '), QString::SkipEmptyParts),
            QIODevice::ReadWrite | QIODevice::Text);
    else
#endif
        d->process.start(d->programPath,
                         QStringList()
                             << serverName
                             << d->arguments.split(QL1C(' '),
                                                   QString::SkipEmptyParts),
                         QIODevice::ReadWrite | QIODevice::Text);
    if (!d->process.waitForStarted())
        throw Exception(
            tr("Unable to start helper program %1").arg(d->programPath));

    // 10s
    if (!d->server->waitForNewConnection(10000)) {
        Q_FOREACH (const auto &line,
                   d->process.readAllStandardOutput().split('\n'))
            qCDebug(EXPORL_COMMON, "%s", line.data());
        throw Exception(tr("No connection from helper program for name %1")
                            .arg(serverName));
    }
    d->socket = d->server->nextPendingConnection();
}

void SlaveServer::stop()
{
    E_D(SlaveServer);

    // Do not throw in here, called in destructor
    if (d->process.isOpen()) {
        d->server.reset();
        d->process.waitForFinished(-1);
    }
}

void SlaveServer::call(const QByteArray &function, QByteArray *output,
                       const QList<QByteArray> &parameters)
{
    E_D(SlaveServer);

    ProtoSlaveRequest request;
    request.set_function(function.constData(), function.size());
    request.set_output(output != NULL);
    Q_FOREACH (const auto &parameter, parameters)
        request.add_parameter(parameter.constData(), parameter.size());

    QByteArray datagram(request.ByteSize() + 4, 0);
    qToLittleEndian<quint32>(
        request.GetCachedSize(),
        reinterpret_cast<unsigned char *>(datagram.data()));
    request.SerializeWithCachedSizesToArray(
        reinterpret_cast<unsigned char *>(datagram.data()) + 4);

    const int send = d->socket->write(datagram);
    if (send != datagram.size())
        throw Exception(tr("Unable to send command to helper program"));
    d->handleResponse(output);
}
}
