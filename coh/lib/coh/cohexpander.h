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

#ifndef _EXPORL_COH_LIB_COH_COHEXPANDER_H_
#define _EXPORL_COH_LIB_COH_COHEXPANDER_H_

#include "coh.h"

namespace coh
{

class CohSequence;

/** Returns a expanded copy of the input CI command sequence. The caller is
 * responsible for freeing the returned instances.
 *
 * If you remove Coh::Sequence from included, anything in the sequences will not
 * be expanded, i.e. as long as numberRepeats > 0 the sequence will be processed
 * as if numberRepeats = 1.
 *
 * @param sequence CI command sequence to expand
 * @return expanded copy of the input CI command sequence
 *
 * @relates CohSequence
 */

COH_EXPORT CohSequence *
expandCohSequence(const CohSequence *sequence,
                  Coh::CommandTypes included = Coh::AllCommands);

} // namespace coh

#endif
