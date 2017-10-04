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

#ifndef _EXPORL_COH_LIB_COH_COHITERATOR_H_
#define _EXPORL_COH_LIB_COH_COHITERATOR_H_

#include "common/global.h"

#include "coh.h"

namespace coh
{

class CohIteratorPrivate;

// ignores CohMetaData, enters CohSequence
// if cloned, the clone sequence is freed at the end
class COH_EXPORT CohIterator
{
public:
    CohIterator(CohSequence *sequence, bool clone);
    ~CohIterator();

    void toFront();
    bool hasNext() const;
    CohStimulus *next();
    CohStimulus *peekNext() const;

private:
    DECLARE_PRIVATE(CohIterator)

protected:
    DECLARE_PRIVATE_DATA(CohIterator)
};

} // namespace coh

#endif
