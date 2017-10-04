/******************************************************************************
 * Copyright (C) 2008-2016  Michael Hofmann <mh21@mh21.de>                    *
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

#include "coh/cohclient.h"
#include "coh/cohprotoloader.h"

#include "common/commonapplication.h"
#include "common/exception.h"
#include "common/slaveclient.h"
#include "common/utils.h"

#include "protocoh.pb.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

using namespace cmn;
using namespace coh;

class ProtoSlaveCommands : public QObject
{
    Q_OBJECT
public:
    ProtoSlaveCommands();

public Q_SLOTS:
    QByteArray create(const QByteArray &address);
    void destroy(const QByteArray &device);
    void send(const QByteArray &device, const QByteArray &sequence);
    void start(const QByteArray &device, const QByteArray &trigger);
    void stop(const QByteArray &device);
    QByteArray status(const QByteArray &device);
    QByteArray needsReloadForStop(const QByteArray &device);

private:
    QByteArray check(const QByteArray &device);

private:
    unsigned deviceIndex;
    QMap<QByteArray, CohClient *> devices;
};

// ProtoSlaveCommands ==========================================================

ProtoSlaveCommands::ProtoSlaveCommands() : deviceIndex(0)
{
}

QByteArray ProtoSlaveCommands::create(const QByteArray &address)
{
    QByteArray device(QByteArray::number(++deviceIndex));
    devices.insert(device,
                   CohClient::createCohClient(QString::fromUtf8(address)));
    return device;
}

void ProtoSlaveCommands::destroy(const QByteArray &device)
{
    delete devices.take(check(device));
}

void ProtoSlaveCommands::send(const QByteArray &device,
                              const QByteArray &sequenceText)
{
    QScopedPointer<CohSequence> sequence(loadCohSequenceProto(sequenceText));
    devices[check(device)]->send(sequence.data());
}

void ProtoSlaveCommands::start(const QByteArray &device,
                               const QByteArray &trigger)
{
    devices[check(device)]->start(Coh::TriggerMode(
        enumKeyToValue(Coh::staticMetaObject, "TriggerMode", trigger)));
}

void ProtoSlaveCommands::stop(const QByteArray &device)
{
    devices[check(device)]->stop();
}

QByteArray ProtoSlaveCommands::status(const QByteArray &device)
{
    return enumValueToKey(Coh::staticMetaObject, "Status",
                          devices[check(device)]->status());
}

QByteArray ProtoSlaveCommands::needsReloadForStop(const QByteArray &device)
{
    return QVariant(devices[check(device)]->needsReloadForStop()).toByteArray();
}

QByteArray ProtoSlaveCommands::check(const QByteArray &device)
{
    if (!devices.contains(device))
        throw Exception(
            tr("Device %1 not found").arg(QString::fromUtf8(device)));
    return device;
}

int main(int argc, char *argv[])
{
    CommonCoreApplication app(argc, argv);

    ProtoSlaveCommands commands;
    SlaveClient client(&commands, app.arguments().value(1));
    QObject::connect(&client, SIGNAL(disconnected()),
                     QCoreApplication::instance(), SLOT(quit()));
    app.exec();

// qCDebug(EXPORL_COH, "Terminating ourselves...");

#ifdef Q_OS_WIN32
    TerminateProcess(GetCurrentProcess(), -1);
#else
    raise(SIGKILL);
#endif
}

#include "protoslave.moc"
