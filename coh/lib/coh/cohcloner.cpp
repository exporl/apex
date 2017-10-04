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

#include "cohcloner.h"

namespace coh
{

class CohCommandCloner : public CohCommandVisitor
{
public:
    CohCommandCloner(const CohSequence *sequence, Coh::CommandTypes included);

    CohSequence *clone();

    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    CohSequence *sequence;
    QScopedPointer<CohSequence> root;
    CohSequence *current;
    Coh::CommandTypes included;
};

// CohCommandCloner ============================================================

CohCommandCloner::CohCommandCloner(const CohSequence *sequence,
                                   Coh::CommandTypes included)
    : sequence(const_cast<CohSequence *>(sequence)), included(included)
{
}

CohSequence *CohCommandCloner::clone()
{
    current = NULL;
    try {
        sequence->accept(this);
    } catch (...) {
        root.reset();
        throw;
    }
    return root.take();
}

void CohCommandCloner::visit(CohMetaData *command)
{
    if (included.testFlag(Coh::MetaData))
        current->append(new CohMetaData(*command));
}

void CohCommandCloner::visit(CohNullStimulus *command)
{
    if (included.testFlag(Coh::NullStimulus))
        current->append(new CohNullStimulus(*command));
}

void CohCommandCloner::visit(CohBiphasicStimulus *command)
{
    if (included.testFlag(Coh::BiphasicStimulus))
        current->append(new CohBiphasicStimulus(*command));
}

void CohCommandCloner::visit(CohCodacsStimulus *command)
{
    if (included.testFlag(Coh::CodacsStimulus))
        current->append(new CohCodacsStimulus(*command));
}

void CohCommandCloner::visit(CohSequence *command)
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

// Various functions ===========================================================

CohSequence *cloneCohSequence(const CohSequence *sequence,
                              Coh::CommandTypes included)
{
    return CohCommandCloner(sequence, included).clone();
}

} // namespace coh
