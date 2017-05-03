/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 * Documentation Copyright (C) 2002  Free Software Foundation, under GPL      *
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
 * Java-style random generator declaration.
 */

#ifndef _APEX_SRC_LIB_APEXTOOLS_RANDOM_H_
#define _APEX_SRC_LIB_APEXTOOLS_RANDOM_H_

#include "common/global.h"
#include "common/random.h"

#include "global.h"

namespace apex
{

class APEXTOOLS_EXPORT Random: public cmn::Random
{
public:
    Random();
    Random(const Random& other);
    Random(quint64 seed);

    unsigned nextUInt();
    unsigned nextUInt(unsigned n);

    unsigned operator()(unsigned n);

private:
    DECLARE_PRIVATE(cmn::Random)
};

} // namespace cmn

#endif

