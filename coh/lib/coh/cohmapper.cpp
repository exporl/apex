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

#include "common/exception.h"

#include "cohmapper.h"

#include <QCoreApplication>

using namespace cmn;

namespace coh
{

class CohSequenceMapperChannel
{
    Q_DECLARE_TR_FUNCTIONS(CohSequenceMapper)
public:
    CohSequenceMapperChannel()
        : activeElectrode(Coh::Electrode(0)),
          referenceElectrode(Coh::Electrode(0)),
          tLevel(0),
          cLevel(0),
          volume(0)
    {
    }

    CohSequenceMapperChannel(Coh::Electrode active, Coh::Electrode reference,
                             unsigned t, unsigned c, double volume)
        : activeElectrode(active),
          referenceElectrode(reference),
          tLevel(t),
          cLevel(c),
          volume(volume)
    {
    }

    unsigned level(double magnitude)
    {
        if (magnitude < 0.0 || magnitude > 1.0)
            throw Exception(
                tr("Magnitude out of bounds [0, 1]: %1").arg(magnitude));
        return qRound(tLevel + (cLevel - tLevel) * volume * magnitude);
    }

    Coh::Electrode activeElectrode;
    Coh::Electrode referenceElectrode;
    unsigned tLevel;
    unsigned cLevel;
    double volume;
};

class CohSequenceMapperPrivate : public CohCommandVisitor
{
    Q_DECLARE_TR_FUNCTIONS(CohSequenceMapper)
public:
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

    template <typename T>
    bool copyable(const QScopedPointer<T> &command,
                  Coh::CommandProperty missing) const;
    bool copyableChannel(
        const QScopedPointer<CohBiphasicStimulus> &command,
        Coh::CommandProperty missing,
        Coh::CommandProperty instead = Coh::CommandProperty(0)) const;

    CohSequence *sequence;
    Coh::CommandProperties needsMappingResult;

    enum { Check, Map } mode;
    QScopedPointer<CohSequence> root;
    CohSequence *current;

    unsigned has;
    Coh::Electrode referenceElectrode;
    double phaseWidth;
    double phaseGap;
    double period;
    CohSequenceMapper::DefaultTrigger trigger;
    bool firstTriggered;

    QMap<int, CohSequenceMapperChannel> channels;
};

// CohSequenceMapperPrivate ====================================================

template <typename T>
bool CohSequenceMapperPrivate::copyable(const QScopedPointer<T> &command,
                                        Coh::CommandProperty missing) const
{
    return !command->availableProperties(missing) && (has & missing) > 0;
}

bool CohSequenceMapperPrivate::copyableChannel(
    const QScopedPointer<CohBiphasicStimulus> &command,
    Coh::CommandProperty missing, Coh::CommandProperty instead) const
{
    return !command->availableProperties(missing) &&
           command->availableProperties(Coh::ChannelIndex | instead) &&
           channels.contains(command->channelIndex());
}

void CohSequenceMapperPrivate::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // not mapped
}

void CohSequenceMapperPrivate::visit(CohNullStimulus *command)
{
    switch (mode) {
    case Check:
        needsMappingResult |= command->missingProperties();
        break;
    case Map: {
        QScopedPointer<CohNullStimulus> mapped(new CohNullStimulus(*command));
        if (!mapped->isValid()) {
            if (copyable(mapped, Coh::Period))
                mapped->setPeriod(period);
        }
        current->append(mapped.take());
        break;
    }
    default:
        throw Exception(tr("Unknown mapping mode"));
    }
}

void CohSequenceMapperPrivate::visit(CohBiphasicStimulus *command)
{
    switch (mode) {
    case Check:
        needsMappingResult |= command->missingProperties();
        break;
    case Map: {
        QScopedPointer<CohBiphasicStimulus> mapped(
            new CohBiphasicStimulus(*command));
        if (!mapped->isValid()) {
            // first the channel mapping, then the rest
            if (copyableChannel(mapped, Coh::ActiveElectrode))
                mapped->setActiveElectrode(
                    channels[mapped->channelIndex()].activeElectrode);
            if (copyableChannel(mapped, Coh::ReferenceElectrode))
                mapped->setReferenceElectrode(
                    channels[mapped->channelIndex()].referenceElectrode);
            if (copyableChannel(mapped, Coh::CurrentLevel,
                                Coh::ChannelMagnitude))
                mapped->setCurrentLevel(channels[mapped->channelIndex()].level(
                    mapped->channelMagnitude()));
            if (copyable(mapped, Coh::ReferenceElectrode))
                mapped->setReferenceElectrode(referenceElectrode);
            if (copyable(mapped, Coh::PhaseWidth))
                mapped->setPhaseWidth(phaseWidth);
            if (copyable(mapped, Coh::PhaseGap))
                mapped->setPhaseGap(phaseGap);
            if (copyable(mapped, Coh::Period))
                mapped->setPeriod(period);
            if (copyable(mapped, Coh::Trigger)) {
                if (trigger == CohSequenceMapper::DefaultTrigger::AllTriggers)
                    mapped->setTrigger(true);
                else if (trigger ==
                             CohSequenceMapper::DefaultTrigger::FirstTrigger &&
                         !firstTriggered)
                    mapped->setTrigger(true);
                else
                    mapped->setTrigger(false);
                firstTriggered = true;
            }
        }
        current->append(mapped.take());
        break;
    }
    default:
        throw Exception(tr("Unknown mapping mode"));
    }
}

void CohSequenceMapperPrivate::visit(CohCodacsStimulus *command)
{
    Q_UNUSED(command);
    throw Exception(tr("Unable to map Codacs stimulus"));
}

void CohSequenceMapperPrivate::visit(CohSequence *command)
{
    CohSequence *sequence = new CohSequence(command->numberRepeats());
    if (!root)
        root.reset(sequence);
    else
        current->append(sequence);

    QListIterator<CohCommand *> i(*command);
    while (i.hasNext()) {
        current = sequence;
        i.next()->accept(this);
    }
}

// CohSequenceMapper ===========================================================

CohSequenceMapper::CohSequenceMapper(CohSequence *sequence)
    : dataPtr(new CohSequenceMapperPrivate)
{
    E_D(CohSequenceMapper);

    d->sequence = sequence;
    d->has = 0;
    d->trigger = DefaultTrigger::NoTriggers;
}

CohSequenceMapper::~CohSequenceMapper()
{
    delete dataPtr;
}

Coh::CommandProperties CohSequenceMapper::needsMapping()
{
    E_D(CohSequenceMapper);

    if (!d->sequence)
        return 0;

    d->needsMappingResult = 0;
    d->mode = CohSequenceMapperPrivate::Check;
    d->sequence->accept(d);
    return d->needsMappingResult;
}

CohSequence *CohSequenceMapper::map()
{
    E_D(CohSequenceMapper);

    try {
        d->mode = CohSequenceMapperPrivate::Map;
        d->sequence->accept(d);
    } catch (...) {
        d->root.reset();
        throw;
    }
    return d->root.take();
}

void CohSequenceMapper::setDefaultReferenceElectrode(Coh::Electrode reference)
{
    E_D(CohSequenceMapper);

    d->has |= Coh::ReferenceElectrode;
    d->referenceElectrode = reference;
}

void CohSequenceMapper::setDefaultPhaseWidth(double width)
{
    E_D(CohSequenceMapper);

    d->has |= Coh::PhaseWidth;
    d->phaseWidth = width;
}

void CohSequenceMapper::setDefaultPhaseGap(double gap)
{
    E_D(CohSequenceMapper);

    d->has |= Coh::PhaseGap;
    d->phaseGap = gap;
}

void CohSequenceMapper::setDefaultPeriod(double interval)
{
    E_D(CohSequenceMapper);

    d->has |= Coh::Period;
    d->period = interval;
}

void CohSequenceMapper::setChannel(int channel, Coh::Electrode active,
                                   Coh::Electrode reference, unsigned t,
                                   unsigned c, double volume)
{
    E_D(CohSequenceMapper);

    d->channels[channel] =
        CohSequenceMapperChannel(active, reference, t, c, volume);
}

void CohSequenceMapper::setDefaultTrigger(DefaultTrigger trigger)
{
    E_D(CohSequenceMapper);

    d->has |= Coh::Trigger;
    d->trigger = trigger;
    d->firstTriggered = false;
}
}
