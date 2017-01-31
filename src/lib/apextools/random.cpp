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

#include "random.h"

#include <cmath>
#include <climits>

#include <QDateTime>
#include <QMutex>

namespace apex
{

bool Random::deterministic = false;

class RandomPrivate
{
public:
    QMutex lock;

    quint64 seed;
    bool haveNextNextGaussian;
    double nextNextGaussian;
};

Random::Random() :
    d(new RandomPrivate)
{
    if (deterministic)
        setSeed (0);
    else
        setSeed (QDateTime::currentDateTime().toTime_t());
}

void Random::setDeterministic(bool p)
{
    deterministic = p;
}

Random::Random (quint64 seed) :
    d(new RandomPrivate)
{
    setSeed (seed);
}

Random::~Random()
{
    delete d;
}

void Random::nextBytes (char* bytes, unsigned int size) const
{
    unsigned roundedSize = (size / 4) * 4;
    for (unsigned i = 0; i < roundedSize; i += 4)
        *reinterpret_cast<unsigned*> (bytes + i) = next (32);
    if (roundedSize < size) {
        unsigned random = next (32);
        memcpy (bytes + roundedSize, &random, size - roundedSize);
    }
}

bool Random::nextBool() const
{
    return next (1) != 0;
}

double Random::nextDouble() const
{
    return ((quint64 (next (26)) << 27) + next (27)) /
        double (quint64 (1) << 53);
}

double Random::nextDouble (double min, double max) const
{
    return nextDouble() * (max - min) + min;
}

float Random::nextFloat() const
{
    return next (24) / float (1 << 24);
}

float Random::nextFloat (double min, double max) const
{
    return nextFloat() * (max - min) + min;
}

double Random::nextGaussian() const
{
    if (d->haveNextNextGaussian) {
        d->haveNextNextGaussian = false;
        return d->nextNextGaussian;
    } else {
        double v1, v2, s;
        do {
            v1 = 2 * nextDouble() - 1; // between -1.0 and 1.0
            v2 = 2 * nextDouble() - 1; // between -1.0 and 1.0
            s = v1 * v1 + v2 * v2;
        } while (s >= 1 || s == 0);
        double multiplier = sqrt (-2 * log (s) / s);
        d->nextNextGaussian = v2 * multiplier;
        d->haveNextNextGaussian = true;
        return v1 * multiplier;
    }
}

int Random::nextInt() const
{
    return next (32);
}

unsigned Random::nextUInt() const
{
    return next (32);
}

unsigned Random::nextUInt (unsigned int n) const
{
    // n is a power of 2
    /*if ((n & -int (n)) == n) // cast silences warning
        return (n * quint64 (next (32))) >> 32;*/
    unsigned bits, val;
    do {
        bits = next (32);
        val = bits % n;
    } while (bits - val > UINT_MAX - (n - 1));
    return val;
}

unsigned Random::operator()(unsigned int n) const
{
    return nextUInt(n);
}

qint64 Random::nextLongLong() const
{
    return (quint64 (next (32)) << 32) + next (32);
}

quint64 Random::nextULongLong() const
{
    return (quint64 (next (32)) << 32) + next (32);
}

unsigned Random::nextULongLong (const quint64 n) const
{
    // n is a power of 2
    if ((n & -int (n)) == n) // cast silences warning
        return (n * quint64 (next (32))) >> 32;
    unsigned bits, val;
    do {
        bits = nextULongLong();
        val = bits % n;
    } while (bits - val > ULONG_MAX - (n - 1));
    return val;
}

void Random::setSeed (quint64 seed)
{
    QMutexLocker locker (&d->lock);

    d->seed = (seed ^ Q_UINT64_C (0x5DEECE66D)) & ((Q_UINT64_C (1) << 48) - 1);
    d->haveNextNextGaussian = false;
}

unsigned Random::next (unsigned int bits) const
{
    QMutexLocker locker (&d->lock);

    d->seed = (d->seed * Q_UINT64_C (0x5DEECE66D) + 0xB) &
        ((Q_UINT64_C (1) << 48) - 1);
    return d->seed >> (48 - bits);
}

} // namespace rba
