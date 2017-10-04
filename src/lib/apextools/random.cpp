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

#include "common/random.h"
#include "common/random_private.h"

#include "random.h"

#include <QDateTime>
#include <QMutex>

#include <climits>
#include <cmath>

namespace apex
{

// Random ======================================================================

Random::Random()
{
}

Random::Random(quint64 seed) : cmn::Random(seed)
{
}

Random::Random(const Random &other) : cmn::Random()
{
    E_D(cmn::Random);

    d->seed = other.dataPtr->seed;
    d->haveNextNextGaussian = other.dataPtr->haveNextNextGaussian;
    d->nextNextGaussian = other.dataPtr->nextNextGaussian;
}

// TODO: needed for MSVC 2010, can be removed on upgrade
unsigned Random::nextUInt()
{
    return cmn::Random::nextUInt();
}

unsigned Random::nextUInt(unsigned n)
{
    /* TODO
     * this seems to be needed to pass the multiprocedure testbench???
    // n is a power of 2
    if ((n & -int(n)) == n) // cast silences warning
        return (n * quint64(next(32))) >> 32;
    */
    unsigned bits, val;
    do {
        bits = next(32);
        val = bits % n;
    } while (bits - val > UINT_MAX - (n - 1));
    return val;
}

unsigned Random::operator()(unsigned n)
{
    return nextUInt(n);
}

} // namespace apex
