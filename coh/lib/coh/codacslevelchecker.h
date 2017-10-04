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

#ifndef _EXPORL_COH_LIB_COH_CODACSLEVELCHECKER_H_
#define _EXPORL_COH_LIB_COH_CODACSLEVELCHECKER_H_

#include "coh.h"

namespace coh
{

/** Checks that a Codacs command sequence does not exceed the given amplitude
 * (TODO: rms).
 *
 * @param sequence Codacs command sequence to check
 * @return true if the Codacs command sequence does not exceed the given
 * amplitude and no non-Codacs commands are found, false otherwise
 *
 * @relates CohSequence
 */
COH_EXPORT bool checkCodacsSequenceLevel(coh::CohSequence *sequence,
                                         unsigned amplitude);

} // namespace coh

#endif
