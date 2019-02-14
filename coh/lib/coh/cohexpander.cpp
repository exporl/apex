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

#include "cohexpander.h"
#include "coh.h"

namespace coh
{

class CohCommandExpander : public CohCommandVisitor
{
public:
    CohCommandExpander(const CohSequence *sequence, Coh::CommandTypes included);

    CohSequence *expand();

    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

private:
    CohSequence *sequence;
    CohSequence *root;
    Coh::CommandTypes included;
};

// CohCommandExpander ==========================================================

CohCommandExpander::CohCommandExpander(const CohSequence *sequence,
                                       Coh::CommandTypes included)
    : sequence(const_cast<CohSequence *>(sequence)), included(included)
{
}

CohSequence *CohCommandExpander::expand()
{
    QScopedPointer<CohSequence> result(new CohSequence(1));
    root = result.data();
    sequence->accept(this);
    return result.take();
}

void CohCommandExpander::visit(CohMetaData *command)
{
    if (included.testFlag(Coh::MetaData))
        root->append(new CohMetaData(*command));
}

void CohCommandExpander::visit(CohNullStimulus *command)
{
    if (included.testFlag(Coh::NullStimulus))
        root->append(new CohNullStimulus(*command));
}

void CohCommandExpander::visit(CohBiphasicStimulus *command)
{
    if (included.testFlag(Coh::BiphasicStimulus))
        root->append(new CohBiphasicStimulus(*command));
}

void CohCommandExpander::visit(CohCodacsStimulus *command)
{
    if (included.testFlag(Coh::CodacsStimulus))
        root->append(new CohCodacsStimulus(*command));
}

void CohCommandExpander::visit(CohSequence *command)
{
    unsigned count = included.testFlag(Coh::Sequence)
                         ? command->numberRepeats()
                         : qMin(command->numberRepeats(), 1u);
    for (unsigned i = 0; i < count; ++i) {
        QListIterator<CohCommand *> j(*command);
        while (j.hasNext())
            j.next()->accept(this);
    }
}

// Various functions ===========================================================

CohSequence *expandCohSequence(const CohSequence *sequence,
                               Coh::CommandTypes included)
{
    return CohCommandExpander(sequence, included).expand();
}

} // namespace coh
