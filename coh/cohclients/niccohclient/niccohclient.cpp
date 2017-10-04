/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "coh/coh.h"
#include "coh/cohclient.h"

#include "common/exception.h"

#include <boost/shared_ptr.hpp>

#include <nic/c_interface/CBiphasicStimulus.h>
#include <nic/c_interface/CNullStimulus.h>
#include <nic/c_interface/CSequence.h>
#include <nic/c_interface/CSequenceCommand.h>
#include <nic/c_interface/CStimulusCommand.h>
#include <nic/c_interface/CTriggerCommand.h>

#include <services/c_interface/CNICStreamClient.h>

#include <QStringList>

#include <QtPlugin>

using namespace cmn;
using namespace coh;

/** Creates CI clients for Cochlear's NIC. Device specifiers are of the form
 * "[<ip address>/]<device>[-<implant>]-<instance>,name=value", with:
 * - device: "l34", "sp12", "cps"
 * - implant: "cic3", "cic4"
 * - instance: 0 captures in a log file, higher numbers correspond to USB ports
 * - name=value: no additional configuration supported at the moment.
 *
 * @ingroup ci
 */
class NicCohClientCreator : public QObject, public CohClientCreator
{
    Q_OBJECT
    Q_INTERFACES(coh::CohClientCreator)
    Q_PLUGIN_METADATA(IID "coh.niccohclient")
public:
    /** Creates a new NIC driver instance for a given device. An
     * std::exception is thrown if the driver is unable to setup the device.
     * The caller is responsible of freeing the returned pointer.
     *
     * If you use a device specifier with an instance of "0", the stimuli will
     * be output to "nic_test.xml" in the current directory.
     *
     * @param device device name as obtained from #ciDevices()
     * @return NIC driver instance
     *
     * @throws std::exception on errors
     */
    virtual CohClient *createCohClient(const QString &device);

    /** Returns the list of devices that are known to the NIC driver. Because
     * the NIC driver provides no way of querying the available devices, this
     * list just contains some common specifiers for devices that need not to be
     * available.
     *
     * @return device names
     */
    virtual QStringList cohDevices() const;

    virtual QString cohDriverName() const;
};
#define NIC_FUNC_EH(func) functionErrorHandler(func, #func)
#define NIC_NEW_EH(ptr) newErrorHandler(ptr, #ptr)

class NicCohClient : public CohClient, public CohCommandVisitor
{
public:
    NicCohClient(const QString &settings);
    ~NicCohClient();

    // NicCohClient implementation
    virtual void send(CohSequence *sequence);
    virtual void start(Coh::TriggerMode trigger);
    virtual void stop();
    virtual Coh::Status status() const;
    virtual bool needsReloadForStop() const;

    // CohCommandWarker implementation
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

    static Coh::Status streamStatus(StreamStatus status);
    static TriggerType triggerType(Coh::TriggerMode trigger);
    static void functionErrorHandler(int result, const char *message);
    template <typename T>
    static T newErrorHandler(T ptr, const char *message);

private:
    boost::shared_ptr<void> client;
    SequenceCommandHandle root;
    SequenceCommandHandle current;
    QList<boost::shared_ptr<void>> data;
};

// CohCommandVisitor implementation ============================================

void NicCohClient::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // ignored
}

void NicCohClient::visit(CohNullStimulus *command)
{
    StimulusHandle stimulusHandle =
        NIC_NEW_EH(NullStimulusNew(command->period()));
    data.append(boost::shared_ptr<void>(stimulusHandle, stimulusDelete));

    SequenceCommandHandle commandHandle =
        command->hasTrigger() ? NIC_NEW_EH(TriggerCommandNew(stimulusHandle))
                              : NIC_NEW_EH(StimulusCommandNew(stimulusHandle));
    data.append(boost::shared_ptr<void>(commandHandle, sequenceCommandDelete));

    NIC_FUNC_EH(sequenceAppend(current, commandHandle));
}

void NicCohClient::visit(CohBiphasicStimulus *command)
{
    StimulusHandle stimulusHandle = NIC_NEW_EH(BiphasicStimulusNew(
        command->activeElectrode(), command->referenceElectrode(),
        command->currentLevel(), command->phaseWidth(), command->phaseGap(),
        command->period()));
    data.append(boost::shared_ptr<void>(stimulusHandle, stimulusDelete));

    SequenceCommandHandle commandHandle =
        command->hasTrigger() ? NIC_NEW_EH(TriggerCommandNew(stimulusHandle))
                              : NIC_NEW_EH(StimulusCommandNew(stimulusHandle));
    data.append(boost::shared_ptr<void>(commandHandle, sequenceCommandDelete));

    NIC_FUNC_EH(sequenceAppend(current, commandHandle));
}

void NicCohClient::visit(CohCodacsStimulus *command)
{
    StimulusHandle stimulusHandle = NIC_NEW_EH(BiphasicStimulusNew(
        command->activeElectrode(), CohCodacsStimulus::referenceElectrode(),
        command->currentLevel(), CohCodacsStimulus::phaseWidth(),
        CohCodacsStimulus::phaseGap(), command->period()));
    data.append(boost::shared_ptr<void>(stimulusHandle, stimulusDelete));

    SequenceCommandHandle commandHandle =
        command->hasTrigger() ? NIC_NEW_EH(TriggerCommandNew(stimulusHandle))
                              : NIC_NEW_EH(StimulusCommandNew(stimulusHandle));
    data.append(boost::shared_ptr<void>(commandHandle, sequenceCommandDelete));

    NIC_FUNC_EH(sequenceAppend(current, commandHandle));
}

void NicCohClient::visit(CohSequence *command)
{
    SequenceCommandHandle sequenceHandle =
        NIC_NEW_EH(SequenceNew(command->numberRepeats()));
    data.append(boost::shared_ptr<void>(sequenceHandle, sequenceCommandDelete));
    if (!root)
        root = sequenceHandle;

    SequenceCommandHandle parent = current;

    QListIterator<CohCommand *> i(*command);
    while (i.hasNext()) {
        current = sequenceHandle;
        i.next()->accept(this);
    }

    // appends a copy of the sequence, has to go after the child stimuli
    if (parent)
        NIC_FUNC_EH(sequenceAppend(parent, sequenceHandle));
}

// NicCohClient ================================================================

NicCohClient::NicCohClient(const QString &settings)
{
    QString address;

    Q_FOREACH (const auto &part,
               settings.split(QL1C(','), QString::SkipEmptyParts)) {
        const QString key = part.section(QL1C('='), 0, 0).trimmed();
        const QString value = part.section(QL1C('='), 1).trimmed();
        if (key == QL1S("power")) {
            // ignored, nscSetPowerLevel call was removed for compatibility with
            // NIC 2.0 and because it is not supported for L34 anyway
        } else if (value.isEmpty()) {
            address = key;
        } else {
            throw Exception(tr("Unknown NIC parameter: %1=%2").arg(key, value));
        }
    }
    client.reset(NIC_NEW_EH(NICStreamClientNew(address.toLocal8Bit())),
                 &nscDelete);
}

NicCohClient::~NicCohClient()
{
    try {
        stop();
    } catch (...) {
        // ignore all errrors in the destructor
    }
}

void NicCohClient::send(CohSequence *sequence)
{
    root = NULL;
    current = NULL;
    try {
        sequence->accept(this);
        NIC_FUNC_EH(nscSendData(client.get(), root));
    } catch (...) {
        data.clear();
        throw;
    }
    data.clear();
}

void NicCohClient::start(Coh::TriggerMode trigger)
{
    NIC_FUNC_EH(nscSetStartType(client.get(), triggerType(trigger)));
    NIC_FUNC_EH(nscStartStream(client.get()));
}

void NicCohClient::stop()
{
    NIC_FUNC_EH(nscStopStream(client.get()));
}

Coh::Status NicCohClient::status() const
{
    StreamStatus status;
    NIC_FUNC_EH(nscGetStreamStatus(client.get(), &status));
    return streamStatus(status);
}

bool NicCohClient::needsReloadForStop() const
{
    return true;
}

// Private methods =============================================================

Coh::Status NicCohClient::streamStatus(StreamStatus status)
{
    switch (status) {
    case 0: // NICUnknown/Unknown
        return Coh::Unknown;
    case Streaming:
        return Coh::Streaming;
    case Idle:
        return Coh::Idle;
    case SafetyError:
        return Coh::SafetyError;
    case Error:
        return Coh::Error;
    case Stopped:
        return Coh::Stopped;
    case Waiting:
        return Coh::Waiting;
    case Underflow:
        return Coh::Underflow;
    default:
        qFatal("Unknown stream status %u", status);
    }
    // Silence the compiler
    return Coh::Unknown;
}

TriggerType NicCohClient::triggerType(Coh::TriggerMode trigger)
{
    switch (trigger) {
    case Coh::Immediate:
        return Immediate;
    case Coh::External:
        return External;
    case Coh::Bilateral:
        return Bilateral;
    default:
        qFatal("Unknown trigger type %u", trigger);
    }
    // Silence the compiler
    return Immediate;
}

void NicCohClient::functionErrorHandler(int result, const char *message)
{
    if (result == Call_Failed)
        throw Exception(tr("NIC function failed: %1").arg(QL1S(message)));
}

template <typename T>
T NicCohClient::newErrorHandler(T ptr, const char *message)
{
    if (!ptr)
        throw Exception(tr("NIC new failed: %1").arg(QL1S(message)));
    return ptr;
}

// NicCohClientCreator =========================================================

CohClient *NicCohClientCreator::createCohClient(const QString &device)
{
    return new NicCohClient(device);
}

QStringList NicCohClientCreator::cohDevices() const
{
    return QStringList() << QL1S("l34-0");
}

QString NicCohClientCreator::cohDriverName() const
{
    return QL1S("nic");
}

#include "niccohclient.moc"
