/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

#include "cohlength.h"

namespace coh
{

class CohCommandLengthCalculator : public CohCommandVisitor
{
public:
    CohCommandLengthCalculator(CohCommand *command);

    double length();

    virtual void visit(CohRfFreeStimulus *command);
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    CohCommand *command;
    double result;
};

// CohCommandLengthCalculator ==================================================

CohCommandLengthCalculator::CohCommandLengthCalculator(CohCommand *command)
    : command(command)
{
}

double CohCommandLengthCalculator::length()
{
    if (!command)
        return 0;

    result = 0;
    command->accept(this);
    return result;
}

void CohCommandLengthCalculator::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // ignored
}

void CohCommandLengthCalculator::visit(CohRfFreeStimulus *command)
{
    result += command->period();
}

void CohCommandLengthCalculator::visit(CohNullStimulus *command)
{
    result += command->period();
}

void CohCommandLengthCalculator::visit(CohBiphasicStimulus *command)
{
    result += command->period();
}

void CohCommandLengthCalculator::visit(CohCodacsStimulus *command)
{
    result += command->period();
}

void CohCommandLengthCalculator::visit(CohSequence *command)
{
    const double oldResult = result;
    result = 0;
    QListIterator<CohCommand *> i(*command);
    while (i.hasNext())
        i.next()->accept(this);
    result = oldResult + command->numberRepeats() * result;
}

// Various functions ===========================================================

double ciSequenceLength(CohSequence *sequence)
{
    return CohCommandLengthCalculator(sequence).length();
}

} // namespace coh
