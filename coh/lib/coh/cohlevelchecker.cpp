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

#include "cohlevelchecker.h"
#include "coh.h"

#include <QtDebug>

namespace coh
{

class CohCommandLevelChecker : public CohCommandVisitor
{
public:
    CohCommandLevelChecker(CohCommand *command);

    bool check(unsigned comfortLevel);
    bool check(const QMap<QPair<Coh::Electrode, Coh::Electrode>, unsigned>
                   &electrodes);

    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    CohCommand *command;
    bool checkElectrodes;
    bool result;
    unsigned comfortLevel;
    QMap<QPair<Coh::Electrode, Coh::Electrode>, unsigned> electrodes;
};

// CohCommandLevelChecker ======================================================

CohCommandLevelChecker::CohCommandLevelChecker(CohCommand *command)
    : command(command)
{
}

bool CohCommandLevelChecker::check(unsigned comfortLevel)
{
    if (!command)
        return true;

    checkElectrodes = false;
    this->comfortLevel = comfortLevel;
    result = true;
    command->accept(this);
    return result;
}

bool CohCommandLevelChecker::check(
    const QMap<QPair<Coh::Electrode, Coh::Electrode>, unsigned> &electrodes)
{
    if (!command)
        return true;

    this->electrodes = electrodes;
    checkElectrodes = true;
    result = true;
    command->accept(this);
    return result;
}

void CohCommandLevelChecker::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // ignored
}

void CohCommandLevelChecker::visit(CohNullStimulus *command)
{
    if (!command->isValid()) {
        this->result = false;
        return;
    }
}

void CohCommandLevelChecker::visit(CohBiphasicStimulus *command)
{
    if (!command->isValid()) {
        this->result = false;
        return;
    }

    const unsigned currentLevel = command->currentLevel();
    if (!checkElectrodes) {
        if (currentLevel > this->comfortLevel)
            this->result = false;
        return;
    }

    const QPair<Coh::Electrode, Coh::Electrode> e =
        qMakePair(command->activeElectrode(), command->referenceElectrode());
    const QPair<Coh::Electrode, Coh::Electrode> e2 =
        qMakePair(command->referenceElectrode(), command->activeElectrode());
    const unsigned comfortLevel =
        electrodes.contains(e) ? electrodes.value(e) : electrodes.value(e2);
    if (currentLevel > comfortLevel)
        this->result = false;
}

void CohCommandLevelChecker::visit(CohCodacsStimulus *command)
{
    Q_UNUSED(command);

    this->result = false;
}

void CohCommandLevelChecker::visit(CohSequence *command)
{
    QListIterator<CohCommand *> i(*command);
    while (i.hasNext() && this->result)
        i.next()->accept(this);
}

// Various functions ===========================================================

bool checkCohSequenceLevel(CohSequence *sequence, unsigned comfortLevel)
{
    return CohCommandLevelChecker(sequence).check(comfortLevel);
}

bool checkCohSequenceLevel(
    CohSequence *sequence,
    const QMap<QPair<Coh::Electrode, Coh::Electrode>, unsigned> &electrodes)
{
    return CohCommandLevelChecker(sequence).check(electrodes);
}

} // namespace coh
