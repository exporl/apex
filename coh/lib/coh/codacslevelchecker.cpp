/******************************************************************************
 * Copyright (C) 2013  Michael Hofmann <mh21@mh21.de>                         *
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

#include "codacslevelchecker.h"
#include "coh.h"

namespace coh
{

class CodacsCommandLevelChecker : public CohCommandVisitor
{
public:
    CodacsCommandLevelChecker(CohCommand *command);

    bool check(unsigned amplitude);

    virtual void visit(CohRfFreeStimulus *command);
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    CohCommand *command;
    bool result;
    unsigned amplitude;
};

// CodacsCommandLevelChecker ===================================================

CodacsCommandLevelChecker::CodacsCommandLevelChecker(CohCommand *command)
    : command(command)
{
}

bool CodacsCommandLevelChecker::check(unsigned amplitude)
{
    if (!this->command)
        return true;

    this->amplitude = amplitude;
    this->result = true;
    command->accept(this);
    return this->result;
}

void CodacsCommandLevelChecker::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // ignored
}

void CodacsCommandLevelChecker::visit(CohRfFreeStimulus *command)
{
    Q_UNUSED(command);

    this->result = false;
}

void CodacsCommandLevelChecker::visit(CohNullStimulus *command)
{
    Q_UNUSED(command);

    this->result = false;
}

void CodacsCommandLevelChecker::visit(CohBiphasicStimulus *command)
{
    Q_UNUSED(command);

    this->result = false;
}

void CodacsCommandLevelChecker::visit(CohCodacsStimulus *command)
{
    if (unsigned(qAbs(command->amplitude())) > this->amplitude)
        this->result = false;
}

void CodacsCommandLevelChecker::visit(CohSequence *command)
{
    QListIterator<CohCommand *> i(*command);
    while (i.hasNext() && this->result)
        i.next()->accept(this);
}

// Various functions ===========================================================

bool checkCodacsSequenceLevel(CohSequence *sequence, unsigned amplitude)
{
    return CodacsCommandLevelChecker(sequence).check(amplitude);
}

} // namespace coh
