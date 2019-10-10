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

#include "common/global.h"

#include "cohtextdumper.h"

#include <limits>

static QString textIndent(unsigned depth, unsigned indentSize)
{
    return QString(indentSize * depth, QL1C(' '));
}

namespace coh
{

class CohTextDumper : public CohCommandVisitor
{
public:
    CohTextDumper(const CohCommand *command);

    QString dump();

    virtual void visit(CohRfFreeStimulus *command);
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    CohCommand *command;
    QString result;
    unsigned indent;
    const unsigned indentSize;
    double nan;
};

// CohTextDumper ===============================================================

CohTextDumper::CohTextDumper(const CohCommand *command)
    : command(const_cast<CohCommand *>(command)),
      indent(0),
      indentSize(2),
      nan(std::numeric_limits<double>::quiet_NaN())
{
}

QString CohTextDumper::dump()
{
    if (!command)
        return QString();

    result.clear();
    command->accept(this);
    return result;
}

void CohTextDumper::visit(CohMetaData *command)
{
    result += QString::fromLatin1("%1Meta: key %2 type %3 value %4\n")
                  .arg(textIndent(indent, indentSize))
                  .arg(command->key(), command->valueType(),
                       command->value().toString());
}

void CohTextDumper::visit(CohRfFreeStimulus *command)
{
    result +=
        QString::fromLatin1("%1Rffree: period %2\n")
            .arg(textIndent(indent, indentSize))
            .arg(command->availableProperties(Coh::Period) ? command->period()
                                                           : nan,
                 0, 'f', 1);
}

void CohTextDumper::visit(CohNullStimulus *command)
{
    result +=
        QString::fromLatin1("%1Null: period %2 trigger %3\n")
            .arg(textIndent(indent, indentSize))
            .arg(command->availableProperties(Coh::Period) ? command->period()
                                                           : nan,
                 0, 'f', 1)
            .arg(command->availableProperties(Coh::Trigger)
                     ? double(command->hasTrigger())
                     : nan);
}

void CohTextDumper::visit(CohBiphasicStimulus *command)
{
    result +=
        QString::fromLatin1(
            "%1Biphasic: active %2 reference %3 level %4 "
            "width %5 gap %6 period %7 channel %8 magnitude %9 trigger %10\n")
            .arg(textIndent(indent, indentSize))
            .arg(command->availableProperties(Coh::ActiveElectrode)
                     ? double(command->activeElectrode())
                     : nan)
            .arg(command->availableProperties(Coh::ReferenceElectrode)
                     ? double(command->referenceElectrode())
                     : nan)
            .arg(command->availableProperties(Coh::CurrentLevel)
                     ? double(command->currentLevel())
                     : nan)
            .arg(command->availableProperties(Coh::PhaseWidth)
                     ? command->phaseWidth()
                     : nan,
                 0, 'f', 1)
            .arg(command->availableProperties(Coh::PhaseGap)
                     ? command->phaseGap()
                     : nan,
                 0, 'f', 1)
            .arg(command->availableProperties(Coh::Period) ? command->period()
                                                           : nan,
                 0, 'f', 1)
            .arg(command->availableProperties(Coh::ChannelIndex)
                     ? double(command->channelIndex())
                     : nan)
            .arg(command->availableProperties(Coh::ChannelMagnitude)
                     ? command->channelMagnitude()
                     : nan,
                 0, 'f', 1)
            .arg(command->availableProperties(Coh::Trigger)
                     ? double(command->hasTrigger())
                     : nan);
}

void CohTextDumper::visit(CohCodacsStimulus *command)
{
    result +=
        QString::fromLatin1("%1Codacs: amplitude %2 period %3 trigger %4\n")
            .arg(textIndent(indent, indentSize))
            .arg(command->amplitude())
            .arg(command->period(), 0, 'f', 1)
            .arg(command->hasTrigger());
}

void CohTextDumper::visit(CohSequence *command)
{
    result += QString::fromLatin1("%1Sequence: repeats %2\n")
                  .arg(textIndent(indent, indentSize))
                  .arg(command->numberRepeats());
    ++indent;
    QListIterator<CohCommand *> i(*command);
    while (i.hasNext())
        i.next()->accept(this);
    --indent;
}

// Various functions ===========================================================

QString dumpCohSequenceText(const CohSequence *sequence)
{
    return CohTextDumper(sequence).dump();
}

} // namespace coh
