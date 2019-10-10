/******************************************************************************
 * Copyright (C) 2019  Michael Hofmann <mh21@mh21.de>                         *
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

#include <deque>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>

#include <nic/nic.h>
#include <nic/nic_eh.hpp>

#include <QStringList>

#include <QtPlugin>

#include "coh/coh.h"
#include "coh/cohclient.h"

using namespace coh;

struct Nic4Deleter {
    void operator()(nic_properties_t t)
    {
        nic_properties_delete(t);
    }
    void operator()(nic_streamer_t t)
    {
        nic_streamer_delete(t);
    }
    void operator()(nic_stimulus_t t)
    {
        nic_stimulus_delete(t);
    }
    void operator()(nic_sequence_command_t t)
    {
        nic_sequence_command_delete(t);
    }
};

typedef std::unique_ptr<_nic_properties_t, Nic4Deleter> Nic4Properties;
typedef std::unique_ptr<_nic_streamer_t, Nic4Deleter> Nic4Streamer;
typedef std::unique_ptr<_nic_stimulus_t, Nic4Deleter> Nic4Stimulus;
typedef std::unique_ptr<_nic_sequence_command_t, Nic4Deleter>
    Nic4SequenceCommand;

/** Creates CI clients for Cochlear's NIC 4.0/4.1. Device specifiers are of the
 * form
 * "property=value,...", corresponding to the properties passed to the streamer
 * on
 * construction.
 */
class Nic4CohClientCreator : public QObject, public CohClientCreator
{
    Q_OBJECT
    Q_INTERFACES(coh::CohClientCreator)
    Q_PLUGIN_METADATA(IID "coh.nic4cohclient")
public:
    /** Creates a new NIC 4 streamer instance for a given device configuration.
     * An std::exception is thrown if something goes wrong. The caller is
     * responsible of freeing the returned pointer.
     *
     * @param device device configuration as obtained from #cohDevices()
     * @return NIC 4 streamer instance
     *
     * @throws std::exception on errors
     */
    virtual CohClient *createCohClient(const QString &device);

    /** Returns the list of devices that are known from NIC 4. Because NIC 4
     * provides no way of querying the available devices, this list just
     * contains some common specifiers for devices that need not to be
     * available.
     *
     * @return device names
     */
    virtual QStringList cohDevices() const;

    virtual QString cohDriverName() const;
};

class Nic4CohClient : public CohClient, public CohCommandVisitor
{
public:
    Nic4CohClient(const QString &settings);

    // NicCohClient implementation
    virtual void send(CohSequence *sequence);
    virtual void start(Coh::TriggerMode trigger);
    virtual void stop();
    virtual Coh::Status status() const;
    virtual bool needsReloadForStop() const;

    // CohCommandWorker implementation
    virtual void visit(CohRfFreeStimulus *command);
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    Nic4Streamer client;
    std::deque<Nic4SequenceCommand> queuedData;

    Nic4SequenceCommand visitorRoot;
    nic_sequence_command_t visitorCurrent; // no ownership
};

// CohCommandVisitor implementation ============================================

void Nic4CohClient::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // ignored
}

void Nic4CohClient::visit(CohRfFreeStimulus *command)
{
    Nic4Stimulus stimulus;
    NIC_EH(stimulus, Nic4Stimulus(nic_rf_free_stimulus_new(command->period())));

    Nic4SequenceCommand sequenceCommand;
    NIC_EH(sequenceCommand,
           Nic4SequenceCommand(nic_stimulus_command_new(stimulus.get())));

    nic_success_code_t result;
    NIC_EH(result, nic_sequence_append(visitorCurrent, sequenceCommand.get()));
}

void Nic4CohClient::visit(CohNullStimulus *command)
{
    Nic4Stimulus stimulus;
    NIC_EH(stimulus, Nic4Stimulus(nic_null_stimulus_new(command->period())));

    Nic4SequenceCommand sequenceCommand;
    if (command->hasTrigger()) {
        NIC_EH(sequenceCommand,
               Nic4SequenceCommand(nic_trigger_command_new(stimulus.get())));
    } else {
        NIC_EH(sequenceCommand,
               Nic4SequenceCommand(nic_stimulus_command_new(stimulus.get())));
    }

    nic_success_code_t result;
    NIC_EH(result, nic_sequence_append(visitorCurrent, sequenceCommand.get()));
}

void Nic4CohClient::visit(CohBiphasicStimulus *command)
{
    Nic4Stimulus stimulus;
    NIC_EH(stimulus,
           Nic4Stimulus(nic_biphasic_stimulus_new(
               command->activeElectrode(), command->referenceElectrode(),
               command->currentLevel(), command->phaseWidth(),
               command->phaseGap(), command->period())));

    Nic4SequenceCommand sequenceCommand;
    if (command->hasTrigger()) {
        NIC_EH(sequenceCommand,
               Nic4SequenceCommand(nic_trigger_command_new(stimulus.get())));
    } else {
        NIC_EH(sequenceCommand,
               Nic4SequenceCommand(nic_stimulus_command_new(stimulus.get())));
    }

    nic_success_code_t result;
    NIC_EH(result, nic_sequence_append(visitorCurrent, sequenceCommand.get()));
}

void Nic4CohClient::visit(CohCodacsStimulus *command)
{
    Q_UNUSED(command);
    throw std::runtime_error("Codacs stimuli are not supported");
}

void Nic4CohClient::visit(CohSequence *command)
{
    nic_sequence_command_t parent = visitorCurrent;

    Nic4SequenceCommand sequenceCommand;
    NIC_EH(sequenceCommand,
           Nic4SequenceCommand(nic_sequence_new(command->numberRepeats())));
    nic_sequence_command_t currentSequence =
        sequenceCommand.get(); // no ownership
    if (!visitorRoot)
        std::swap(visitorRoot, sequenceCommand); // move ownership to send()

    QListIterator<CohCommand *> i(*command);
    while (i.hasNext()) {
        visitorCurrent = currentSequence;
        i.next()->accept(this);
    }

    // appends a copy of the sequence, has to go after the child stimuli
    nic_success_code_t result;
    if (parent)
        NIC_EH(result, nic_sequence_append(parent, currentSequence));
}

// Nic4CohClient
// ================================================================

Nic4CohClient::Nic4CohClient(const QString &settings)
{
    const static std::map<std::string, const char *> defaults = {{
        {"platform", "SP16"},
        {"auto_pufs", "off"},
        {"flagged_electrodes", ""},
        {"min_pulse_width_us", "25"},
        {"c_levels", "255 255 255 255 255 255 255 255 255 255 255 255 255 255 "
                     "255 255 255 255 255 255 255 255"},
        {"c_levels_pulse_width_us", "1000"},
    }};
    std::set<std::string> seen;

    Nic4Properties properties;
    NIC_EH(properties, Nic4Properties(nic_properties_new()));

    nic_success_code_t result;
    Q_FOREACH (const auto &part,
               settings.split(QL1C(','), QString::SkipEmptyParts)) {
        const QString key = part.section(QL1C('='), 0, 0).trimmed();
        const QString value = part.section(QL1C('='), 1).trimmed();
        NIC_EH(result, nic_properties_add(properties.get(), key.toUtf8(),
                                          value.toUtf8()));
        seen.insert(key.toStdString());
    }
    for (const auto &d : defaults) {
        if (!seen.count(d.first))
            NIC_EH(result, nic_properties_add(properties.get(), d.first.c_str(),
                                              d.second));
    }

    NIC_EH(client, Nic4Streamer(nic_streamer_new(properties.get())));
    NIC_EH(result, nic_streamer_start(client.get()));
}

void Nic4CohClient::send(CohSequence *sequence)
{
    visitorRoot = nullptr;
    visitorCurrent = nullptr;
    try {
        sequence->accept(this);
        queuedData.push_back(std::move(visitorRoot));
    } catch (...) {
        visitorRoot.reset();
        throw;
    }
}

void Nic4CohClient::start(Coh::TriggerMode trigger)
{
    if (trigger != Coh::Immediate)
        throw std::runtime_error("Only immediate start is supported");

    nic_success_code_t result;
    while (!queuedData.empty()) {
        NIC_EH(result, nic_streamer_send_data(client.get(),
                                              queuedData.front().get(), false));
        queuedData.pop_front();
    }
}

void Nic4CohClient::stop()
{
    nic_success_code_t result;
    NIC_EH(result, nic_streamer_send_data(client.get(), nullptr, true));
}

Coh::Status Nic4CohClient::status() const
{
    nic_success_code_t result;
    int finished;
    NIC_EH(result, nic_streamer_is_finished(client.get(), &finished));
    return finished ? Coh::Idle : Coh::Streaming;
}

bool Nic4CohClient::needsReloadForStop() const
{
    return false;
}

// Nic4CohClientCreator
// =========================================================

CohClient *Nic4CohClientCreator::createCohClient(const QString &device)
{
    return new Nic4CohClient(device);
}

QStringList Nic4CohClientCreator::cohDevices() const
{
    return QStringList({
        QSL("implant=CIC4,min_pulse_width_us=25.0,go_live=on,mode=MP1+2"),
        QSL("implant=CIC4,min_pulse_width_us=25.0,go_live=on,mode=BP"),
        QSL("implant=CIC4,min_pulse_width_us=12.0,go_live=on,mode=MP1+2"),
    });
}

QString Nic4CohClientCreator::cohDriverName() const
{
    return QL1S("nic4");
}

#include "nic4cohclient.moc"
