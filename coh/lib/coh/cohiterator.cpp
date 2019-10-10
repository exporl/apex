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

#include "cohiterator.h"
#include "cohcloner.h"

#include <QSharedPointer>
#include <QStack>

namespace coh
{

class CohIteratorPrivate : public CohCommandVisitor
{
public:
    // CohCommandVisitor implementation
    virtual void visit(CohRfFreeStimulus *command);
    virtual void visit(CohNullStimulus *command);
    virtual void visit(CohBiphasicStimulus *command);
    virtual void visit(CohCodacsStimulus *command);
    virtual void visit(CohSequence *command);
    virtual void visit(CohMetaData *command);

    bool iterate();

    QSharedPointer<CohSequence> sequence;
    CohStimulus *currentStimulus;
    QStack<CohSequence *> sequenceStack;
    QStack<unsigned> posStack;
    QStack<unsigned> loopStack;
};

// CohIteratorPrivate ==========================================================

void CohIteratorPrivate::visit(CohMetaData *command)
{
    Q_UNUSED(command);
    // ignored
}

void CohIteratorPrivate::visit(CohRfFreeStimulus *command)
{
    currentStimulus = command;
}

void CohIteratorPrivate::visit(CohNullStimulus *command)
{
    currentStimulus = command;
}

void CohIteratorPrivate::visit(CohBiphasicStimulus *command)
{
    currentStimulus = command;
}

void CohIteratorPrivate::visit(CohCodacsStimulus *command)
{
    currentStimulus = command;
}

void CohIteratorPrivate::visit(CohSequence *command)
{
    sequenceStack.push(command);
    posStack.push(0);
    loopStack.push(0);
}

bool CohIteratorPrivate::iterate()
{
    if (sequenceStack.isEmpty())
        return false;

    CohSequence *currentSequence = sequenceStack.pop();
    unsigned pos = posStack.pop();
    unsigned loop = loopStack.pop();

    if (pos >= unsigned(currentSequence->size())) {
        ++loop;
        pos = 0;
    }
    if (pos < unsigned(currentSequence->size()) &&
        loop < currentSequence->numberRepeats()) {
        CohCommand *current = currentSequence->at(pos);

        // Save next position
        sequenceStack.push(currentSequence);
        posStack.push(++pos);
        loopStack.push(loop);

        current->accept(this);
    }

    return true;
}

// CohIterator =================================================================

CohIterator::CohIterator(CohSequence *sequence, bool clone)
    : dataPtr(new CohIteratorPrivate)
{
    E_D(CohIterator);

    if (clone) {
        d->sequence = QSharedPointer<CohSequence>(cloneCohSequence(sequence));
    } else {
        d->sequence = QSharedPointer<CohSequence>(sequence, [](void *) {});
    }
    d->currentStimulus = NULL;
    toFront();
}

CohIterator::~CohIterator()
{
    delete dataPtr;
}

void CohIterator::toFront()
{
    E_D(CohIterator);

    d->sequenceStack.clear();
    d->posStack.clear();
    d->loopStack.clear();

    d->sequenceStack.push(d->sequence.data());
    d->posStack.push(0);
    d->loopStack.push(0);

    next();
}

bool CohIterator::hasNext() const
{
    E_D(const CohIterator);

    return !d->sequenceStack.isEmpty();
}

CohStimulus *CohIterator::next()
{
    E_D(CohIterator);

    CohStimulus *result = d->currentStimulus;

    d->currentStimulus = NULL;
    while (!d->currentStimulus)
        if (!d->iterate())
            break;

    return result;
}

CohStimulus *CohIterator::peekNext() const
{
    E_D(const CohIterator);

    return d->currentStimulus;
}

} // namespace coh
