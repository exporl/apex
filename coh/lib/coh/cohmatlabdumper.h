/******************************************************************************
 * Copyright (C) 2008,2015  Michael Hofmann <mh21@mh21.de>                    *
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

/** @file
 * CI text dump declaration.
 */

#ifndef _EXPORL_COH_LIB_COH_COHMATLABDUMPER_H_
#define _EXPORL_COH_LIB_COH_COHMATLABDUMPER_H_

#include "coh.h"

#include <QString>

namespace coh
{

class CohSequence;

/** Returns a matlab-readable representation of a CI command sequence.
 *
 * @param sequence CI command sequence to dump
 * @return matlab-readable representation of the CI command sequence
 *
 * @relates CohSequence
 */
COH_EXPORT QString dumpCohSequenceMatlab(const CohSequence *sequence);

} // namespace coh

#endif
