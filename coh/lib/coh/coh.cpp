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

#include "common/global.h"

#include "coh.h"

#include <QString>

#include <cmath>

// Q_LOGGING_CATEGORY(EXPORL_COH, "exporl.coh")
// TODO: workaround for non-constness on Qt 5.2, remove after switch to 16.04
// LTS
const QLoggingCategory &EXPORL_COH()
{
    static QLoggingCategory category("exporl.coh");
    return category;
}

namespace coh
{

// CohCommand ==================================================================

CohCommand::CohCommand() : properties(0), required(0)
{
}

CohCommand::CohCommand(Coh::CommandProperties properties,
                       Coh::CommandProperties required)
    : properties(properties), required(required)
{
}

CohCommand::~CohCommand()
{
}

Coh::CommandProperties CohCommand::availableProperties() const
{
    return properties;
}

bool CohCommand::availableProperties(Coh::CommandProperties tested) const
{
    return (properties & tested) == tested;
}

Coh::CommandProperties CohCommand::requiredProperties() const
{
    return required;
}

Coh::CommandProperties CohCommand::missingProperties() const
{
    return ~properties & required;
}

bool CohCommand::isValid() const
{
    return (properties & required) == required;
}

// CohMetaData =================================================================

CohMetaData::CohMetaData(const QString &key, const QVariant &value)
    : first(key), second(value)
{
}

CohMetaData::CohMetaData(const QString &key, const QString &type,
                         const QString &value)
    : first(key), second(value)
{
    if (type != QL1S("string"))
        second.convert(QVariant::nameToType(type.toLatin1()));
}

QString CohMetaData::key() const
{
    return first;
}

void CohMetaData::setKey(const QString &value)
{
    first = value;
}

QVariant CohMetaData::value() const
{
    return second;
}

QString CohMetaData::valueType() const
{
    QString result = QString::fromLatin1(second.typeName());
    if (result == QL1S("QString"))
        result = QL1S("string");
    return result;
}

void CohMetaData::setValue(const QVariant &value)
{
    second = value;
}

void CohMetaData::setValue(const QString &type, const QString &value)
{
    second = value;
    if (type != QL1S("string"))
        second.convert(QVariant::nameToType(type.toLatin1()));
}

void CohMetaData::accept(CohCommandVisitor *visitor)
{
    visitor->visit(this);
}

// CohStimulus =================================================================

bool CohStimulus::roundToRfCycles = true;

CohStimulus::CohStimulus(Coh::CommandProperties properties,
                         Coh::CommandProperties required, double interval,
                         bool trigger)
    : CohCommand(properties, required),
      trigger(trigger),
      interval(roundToRfCycles ? roundedCohPeriod(interval) : interval)
{
}

bool CohStimulus::hasTrigger() const
{
    return trigger;
}

void CohStimulus::setTrigger(bool trigger)
{
    this->properties |= Coh::Trigger;
    this->trigger = trigger;
}

double CohStimulus::period() const
{
    return interval;
}

void CohStimulus::setPeriod(double interval)
{
    this->properties |= Coh::Period;
    this->interval = roundToRfCycles ? roundedCohPeriod(interval) : interval;
}

void CohStimulus::setRoundToRfCycles(bool round)
{
    CohStimulus::roundToRfCycles = round;
}

// CohNullStimulus =============================================================

CohNullStimulus::CohNullStimulus(double interval, bool trigger)
    : CohStimulus(Coh::NullProperties, Coh::NullProperties, interval, trigger)
{
}

CohNullStimulus::CohNullStimulus()
    : CohStimulus(0, Coh::NullProperties, 0, false)
{
}

CohNullStimulus *CohNullStimulus::incompleteStimulus()
{
    return new CohNullStimulus();
}

void CohNullStimulus::accept(CohCommandVisitor *visitor)
{
    visitor->visit(this);
}

// CohBiphasicStimulus =========================================================

CohBiphasicStimulus::CohBiphasicStimulus(Coh::Electrode active,
                                         Coh::Electrode reference,
                                         unsigned level, double width,
                                         double gap, double interval,
                                         bool trigger)
    : CohStimulus(Coh::BiphasicProperties, Coh::BiphasicProperties, interval,
                  trigger),
      active(active),
      reference(reference),
      level(level),
      width(roundToRfCycles ? roundedCohPeriod(width) : width),
      gap(roundToRfCycles ? roundedCohPeriod(gap) : gap),
      channel(0),
      magnitude(0)
{
}

CohBiphasicStimulus::CohBiphasicStimulus()
    : CohStimulus(0, Coh::BiphasicProperties, 0, false),
      active(Coh::CI_ELECTRODE_IC0),
      reference(Coh::CI_ELECTRODE_IC0),
      level(0),
      width(0),
      gap(0),
      channel(0),
      magnitude(0)
{
}

CohBiphasicStimulus *CohBiphasicStimulus::incompleteStimulus()
{
    return new CohBiphasicStimulus();
}

Coh::Electrode CohBiphasicStimulus::activeElectrode() const
{
    return active;
}

void CohBiphasicStimulus::setActiveElectrode(Coh::Electrode active)
{
    this->properties |= Coh::ActiveElectrode;
    this->active = active;
}

Coh::Electrode CohBiphasicStimulus::referenceElectrode() const
{
    return reference;
}

void CohBiphasicStimulus::setReferenceElectrode(Coh::Electrode reference)
{
    this->properties |= Coh::ReferenceElectrode;
    this->reference = reference;
}

unsigned CohBiphasicStimulus::currentLevel() const
{
    return level;
}

void CohBiphasicStimulus::setCurrentLevel(unsigned level)
{
    this->properties |= Coh::CurrentLevel;
    this->level = level;
}

double CohBiphasicStimulus::phaseWidth() const
{
    return width;
}

void CohBiphasicStimulus::setPhaseWidth(double width)
{
    this->properties |= Coh::PhaseWidth;
    this->width = roundToRfCycles ? roundedCohPeriod(width) : width;
}

double CohBiphasicStimulus::phaseGap() const
{
    return gap;
}

void CohBiphasicStimulus::setPhaseGap(double gap)
{
    this->properties |= Coh::PhaseGap;
    this->gap = roundToRfCycles ? roundedCohPeriod(gap) : gap;
}

double CohBiphasicStimulus::channelMagnitude() const
{
    return magnitude;
}

void CohBiphasicStimulus::setChannelMagnitude(double magnitude)
{
    this->properties |= Coh::ChannelMagnitude;
    this->magnitude = magnitude;
}

unsigned CohBiphasicStimulus::channelIndex() const
{
    return channel;
}

void CohBiphasicStimulus::setChannelIndex(unsigned channel)
{
    this->properties |= Coh::ChannelIndex;
    this->channel = channel;
}

void CohBiphasicStimulus::accept(CohCommandVisitor *visitor)
{
    visitor->visit(this);
}

// CohCodacsStimulus ===========================================================

CohCodacsStimulus::CohCodacsStimulus(int level, double interval, bool trigger)
    : CohStimulus(Coh::CodacsProperties, Coh::CodacsProperties, interval,
                  trigger),
      level(level)
{
}

CohCodacsStimulus::CohCodacsStimulus()
    : CohStimulus(0, Coh::CodacsProperties, 0, false), level(0)
{
}

CohCodacsStimulus *CohCodacsStimulus::incompleteStimulus()
{
    return new CohCodacsStimulus();
}

int CohCodacsStimulus::amplitude() const
{
    return level;
}

int CohCodacsStimulus::expandedAmplitude() const
{
    int sign = level < 0 ? -1 : 1;
    unsigned raw = qAbs(level);

    switch ((raw & 0xf00) >> 8) {
    case 0:
    case 1:
    case 2:
    case 3:
        return sign * 2 * raw;
    case 4:
        return sign * 8 * (raw - 768);
    case 5:
        return sign * 16 * (raw - 1024);
    case 6:
        return sign * 32 * (raw - 1280);
    default: // 7, 8
        return sign * 64 * (raw - 1536);
    }
}

void CohCodacsStimulus::setAmplitude(int level)
{
    this->properties |= Coh::Amplitude;
    this->level = level;
}

Coh::Electrode CohCodacsStimulus::activeElectrode() const
{
    return Coh::Electrode((this->level & 0xf80) >> 7);
}

Coh::Electrode CohCodacsStimulus::referenceElectrode()
{
    return Coh::Electrode(Coh::CI_ELECTRODE_EC1_2);
}

unsigned CohCodacsStimulus::currentLevel() const
{
    return (this->level & 0x07f) << 1;
}

double CohCodacsStimulus::phaseWidth()
{
    return 12.0;
}

double CohCodacsStimulus::phaseGap()
{
    return 6.0;
}

double CohCodacsStimulus::defaultPeriod()
{
    return 51.0;
}

void CohCodacsStimulus::accept(CohCommandVisitor *visitor)
{
    visitor->visit(this);
}

// CohSequence =================================================================

CohSequence::CohSequence(unsigned repeats) : repeats(repeats)
{
}

CohSequence::~CohSequence()
{
    qDeleteAll(*this);
}

unsigned CohSequence::numberRepeats() const
{
    return repeats;
}

void CohSequence::setNumberRepeats(unsigned repeats)
{
    this->repeats = repeats;
}

void CohSequence::accept(CohCommandVisitor *visitor)
{
    visitor->visit(this);
}

// Functions ===================================================================

double cohCurrentForCurrentLevel(unsigned currentLevel, Coh::Type implant)
{
    switch (implant) {
    case Coh::Cic3Implant:
        return 10.0 * pow(175.0, currentLevel / 255.0);
    case Coh::Cic4Implant:
        return 17.5 * pow(100.0, currentLevel / 255.0);
    default:
        return 0.0;
    }
}

unsigned cohCurrentLevelForCurrent(double current, Coh::Type implant)
{
    switch (implant) {
    case Coh::Cic3Implant:
        return qRound(255.0 / log(175.0) * log(current / 10.0));
    case Coh::Cic4Implant:
        return qRound(255.0 / log(100.0) * log(current / 17.5));
    default:
        return 0u;
    }
}

unsigned roundedCohCycles(double period)
{
    return qMax(0, qRound(period * 5));
}

double roundedCohPeriod(double period)
{
    return roundedCohCycles(period) / 5.0;
}

} // namespace coh
