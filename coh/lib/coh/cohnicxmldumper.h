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

/** @file
 * CI XML export declaration.
 */

#ifndef _EXPORL_COH_LIB_COH_COHNICXMLDUMPER_H_
#define _EXPORL_COH_LIB_COH_COHNICXMLDUMPER_H_

#include "coh.h"

#include <QString>

namespace coh
{

/** Saves CI command sequences to XML code compatible with Cochlear's NIC.
 *
 * @param filePath path to the output XML file
 * @param sequence CI command sequence to convert
 *
 * @throws std::exception on errors
 *
 * @relates CohSequence
 */
COH_EXPORT QByteArray dumpCohSequenceNicXml(const CohSequence *sequence);

} // namespace coh

#endif
