/******************************************************************************
 * Copyright (C) 2007-2013  Michael Hofmann <mh21@mh21.de>                    *
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
#include "coh/cohprotodumper.h"

#include "common/exception.h"
#include "common/paths.h"
#include "common/random.h"
#include "common/slaveserver.h"
#include "common/utils.h"

#include "protocoh.pb.h"

#include <QSet>
#include <QStringList>

#include <QtPlugin>

using namespace cmn;
using namespace coh;

/** Creates CI clients that use the protoslave/nic3slave helper programs to do
 * the actual work.
 *
 * For protoslave, the device name is of the form
 * "slave=/path/to/slave,args=cmd args,verbose=true; driver: origdevname".
 * For nic3slave, this is
 * "slave=/path/to/slave.py,args=cmd args,verbose=true; key1=val1,key2=val2...".
 */
class ProtoSlaveCohClientCreator : public QObject, public CohClientCreator
{
    Q_OBJECT
    Q_INTERFACES(coh::CohClientCreator)
    Q_PLUGIN_METADATA(IID "coh.protoslavecohclient")
public:
    /** Creates a new slave driver instance for a given device. An
     * std::exception is thrown if the driver is unable to setup the device.
     * The caller is responsible of freeing the returned pointer.
     *
     * @param device device name as obtained from #ciDevices()
     * @return slave driver instance
     *
     * @throws std::exception on errors
     */
    virtual CohClient *createCohClient(const QString &device);

    /** Returns the list of devices that are known to the slave driver. Because
     * the slave driver has no way of knowing what possible devices exist for
     * the real drivers, this list just contains some common specifiers for
     * devices that need not to be available.
     *
     * @return device names
     */
    virtual QStringList cohDevices() const;

    virtual QString cohDriverName() const;
};

class ProtoSlaveCohClientMultiplexer
{
    Q_DECLARE_TR_FUNCTIONS(ProtoSlaveCohClientMultiplexer)
public:
    ProtoSlaveCohClientMultiplexer();
    ~ProtoSlaveCohClientMultiplexer();

    QByteArray create(const QString &mergedSettings);
    void destroy(const QByteArray &device);
    void send(const QByteArray &device, CohSequence *sequence);
    void start(const QByteArray &device, Coh::TriggerMode trigger);
    void stop(const QByteArray &device);
    Coh::Status status(const QByteArray &device) const;
    bool needsReloadForStop(const QByteArray &device) const;

private:
    QByteArray ensureRpc(const QString &mergedSettings);
    SlaveServer *rpc() const;

private:
    QSet<QByteArray> devices;
    QStringList devicesToReload;
    QScopedPointer<SlaveServer> slaveServer;
};

class ProtoSlaveCohClient : public CohClient
{
public:
    ProtoSlaveCohClient(const QString &mergedSettings);
    ~ProtoSlaveCohClient();

    // CohClient implementation
    virtual void send(CohSequence *sequence);
    virtual void start(Coh::TriggerMode trigger);
    virtual void stop();
    virtual Coh::Status status() const;
    virtual bool needsReloadForStop() const;

private:
    QSharedPointer<ProtoSlaveCohClientMultiplexer> multiplexer;
    QByteArray device;
};

// ProtoSlaveCohClientMultiplexer ==============================================

ProtoSlaveCohClientMultiplexer::ProtoSlaveCohClientMultiplexer()
{
}

ProtoSlaveCohClientMultiplexer::~ProtoSlaveCohClientMultiplexer()
{
    while (devices.size() > 0)
        destroy(*devices.constBegin());
}

QByteArray
ProtoSlaveCohClientMultiplexer::ensureRpc(const QString &mergedSettings)
{
    QString address;
    QString settings;
    QString slave(QL1S("protoslave"));
    QString arguments;
    QString pythonVersion;
    bool verbose = false;

    if (mergedSettings.contains(QL1C(';'))) {
        settings = mergedSettings.section(QL1C(';'), 0, 0).trimmed();
        address = mergedSettings.section(QL1C(';'), 1).trimmed();
    } else {
        address = mergedSettings;
    }

    if (!slaveServer) {
        Q_FOREACH (const auto &part,
                   settings.split(QL1C(','), QString::SkipEmptyParts)) {
            const QString key = part.section(QL1C('='), 0, 0).trimmed();
            const QString value = part.section(QL1C('='), 1).trimmed();
            if (key == QL1S("slave")) {
                slave = value;
            } else if (key == QL1S("args")) {
                arguments = value;
            } else if (key == QL1S("verbose")) {
                verbose = true;
            } else if (key == QL1S("python")) {
                pythonVersion = value;
            } else if (key == QL1S("label")) {
                // ignored
            } else {
                throw Exception(tr("Unknown parameter: %1=%2").arg(key, value));
            }
        }
        slaveServer.reset(
            new SlaveServer(slave, arguments, pythonVersion, verbose));
    }

    return address.toUtf8();
}

QByteArray ProtoSlaveCohClientMultiplexer::create(const QString &mergedSettings)
{
    QByteArray address = ensureRpc(mergedSettings);

    QByteArray device;
    rpc()->call("create", &device, QList<QByteArray>() << address);
    devices.insert(device);

    QByteArray reloadNeeded;
    rpc()->call("needsReloadForStop", &reloadNeeded, QList<QByteArray>()
                                                         << device);
    if (QVariant(reloadNeeded).toBool())
        devicesToReload.append(mergedSettings);

    return device;
}

void ProtoSlaveCohClientMultiplexer::destroy(const QByteArray &device)
{
    if (devices.contains(device)) {
        devices.remove(device);
        try {
            rpc()->call("stop", NULL, QList<QByteArray>() << device);
            rpc()->call("destroy", NULL, QList<QByteArray>() << device);
        } catch (...) {
            // silently ignored, pretend that the device got destroyed
        }
    }
    if (devices.size() == 0) {
        slaveServer.reset();
        while (!devicesToReload.isEmpty()) {
            try {
                QByteArray address = ensureRpc(devicesToReload.takeFirst());
                QByteArray device;
                rpc()->call("create", &device, QList<QByteArray>() << address);
                rpc()->call("destroy", NULL, QList<QByteArray>() << device);
            } catch (...) {
                // silently ignored
            }
            slaveServer.reset();
        }
    }
}

SlaveServer *ProtoSlaveCohClientMultiplexer::rpc() const
{
    if (!slaveServer)
        throw Exception(tr("No slave started"));
    return slaveServer.data();
}

void ProtoSlaveCohClientMultiplexer::send(const QByteArray &device,
                                          CohSequence *sequence)
{
    rpc()->call("send", NULL, QList<QByteArray>()
                                  << device << dumpCohSequenceProto(sequence));
}

void ProtoSlaveCohClientMultiplexer::start(const QByteArray &device,
                                           Coh::TriggerMode trigger)
{
    rpc()->call("start", NULL, QList<QByteArray>()
                                   << device
                                   << enumValueToKey(Coh::staticMetaObject,
                                                     "TriggerMode", trigger));
}

void ProtoSlaveCohClientMultiplexer::stop(const QByteArray &device)
{
    rpc()->call("stop", NULL, QList<QByteArray>() << device);
}

Coh::Status
ProtoSlaveCohClientMultiplexer::status(const QByteArray &device) const
{
    QByteArray result;
    rpc()->call("status", &result, QList<QByteArray>() << device);
    return Coh::Status(enumKeyToValue(Coh::staticMetaObject, "Status", result));
}

bool ProtoSlaveCohClientMultiplexer::needsReloadForStop(
    const QByteArray &device) const
{
    QByteArray result;
    rpc()->call("needsReloadForStop", &result, QList<QByteArray>() << device);
    return QVariant(result).toBool();
}

// ProtoSlaveCohClient =========================================================

ProtoSlaveCohClient::ProtoSlaveCohClient(const QString &mergedSettings)
{
    // only one multiplexer, lifetime managed in same thread as CohClient
    static QWeakPointer<ProtoSlaveCohClientMultiplexer> globalInstance;
    multiplexer = globalInstance.toStrongRef();
    if (!multiplexer) {
        multiplexer.reset(new ProtoSlaveCohClientMultiplexer);
        globalInstance = multiplexer;
    }
    device = multiplexer->create(mergedSettings);
}

ProtoSlaveCohClient::~ProtoSlaveCohClient()
{
    multiplexer->destroy(device);
}

void ProtoSlaveCohClient::send(CohSequence *sequence)
{
    multiplexer->send(device, sequence);
}

void ProtoSlaveCohClient::start(Coh::TriggerMode trigger)
{
    multiplexer->start(device, trigger);
}

void ProtoSlaveCohClient::stop()
{
    multiplexer->stop(device);
}

Coh::Status ProtoSlaveCohClient::status() const
{
    return multiplexer->status(device);
}

bool ProtoSlaveCohClient::needsReloadForStop() const
{
    // any required reload is handled internally
    return false;
}

// ProtoSlaveCohClientCreator ==================================================

CohClient *ProtoSlaveCohClientCreator::createCohClient(const QString &device)
{
    return new ProtoSlaveCohClient(device);
}

QStringList ProtoSlaveCohClientCreator::cohDevices() const
{
    return QStringList() << QL1S("nic: l34-cic3-1") << QL1S("nic: l34-cic4-1")
                         << QL1S("nic: l34-0")
                         << QL1S("label=NIC3.1-L34-CIC4,"
                                 "python=2.5,"
                                 "slave=nic3slave; "
                                 "platform=L34,"
                                 "implant=CIC4,"
                                 "min_pulse_width_us=25.0,"
                                 "auto_pufs=off,"
                                 "go_live=on,"
                                 "latency_ms=1000")
                         << QL1S("label=NIC3.1-L34-Codacs,"
                                 "python=2.5,"
                                 "slave=nic3slave,"
                                 "args=noflexibleframeperiods "
                                 "extendedamplituderange allowimplantcommands; "
                                 "platform=L34,"
                                 "implant=CIC4,"
                                 "min_pulse_width_us=12.0,"
                                 "auto_pufs=off,"
                                 "go_live=on,"
                                 "latency_ms=1000")
                         << QL1S("label=NIC3.2-L34-CIC4,"
                                 "python=2.7,"
                                 "slave=nic3slave; "
                                 "platform=L34,"
                                 "implant=CIC4,"
                                 "mode=MP1+2,"
                                 "min_pulse_width_us=25.0,"
                                 "auto_pufs=off,"
                                 "go_live=on,"
                                 "flagged_electrodes=,"
                                 "latency_ms=1000")
                         << QL1S("label=NIC3.2-L34-Codacs,"
                                 "python=2.7,"
                                 "slave=nic3slave,"
                                 "args=noflexibleframeperiods "
                                 "extendedamplituderange allowimplantcommands; "
                                 "platform=L34,"
                                 "implant=CIC4,"
                                 "mode=MP1+2,"
                                 "min_pulse_width_us=12.0,"
                                 "auto_pufs=off,"
                                 "go_live=on,"
                                 "flagged_electrodes=,"
                                 "latency_ms=1000")
                         << QL1S("label=NIC3.2-RFGenXS-CIC4,"
                                 "python=2.7,"
                                 "slave=nic3slave; "
                                 "platform=RFGenXS,"
                                 "implant=CIC4,"
                                 "mode=MP1+2,"
                                 "min_pulse_width_us=25.0,"
                                 "auto_pufs=off,"
                                 "go_live=on,"
                                 "flagged_electrodes=,"
                                 "latency_ms=1000")
                         << QL1S("label=NIC3.2-L34-Codacs,"
                                 "python=2.7,"
                                 "slave=nic3slave,"
                                 "args=noflexibleframeperiods "
                                 "extendedamplituderange allowimplantcommands; "
                                 "platform=RFGenXS,"
                                 "implant=CIC4,"
                                 "mode=MP1+2,"
                                 "min_pulse_width_us=12.0,"
                                 "auto_pufs=off,"
                                 "go_live=on,"
                                 "flagged_electrodes=,"
                                 "latency_ms=1000");
}

QString ProtoSlaveCohClientCreator::cohDriverName() const
{
    return QL1S("protoslave");
}

#include "protoslavecohclient.moc"
