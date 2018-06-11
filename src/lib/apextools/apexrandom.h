/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _APEX_SRC_LIB_APEXTOOLS_APEXRANDOM_H_
#define _APEX_SRC_LIB_APEXTOOLS_APEXRANDOM_H_
// random function from numerical recipes

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)
#define EPS 1.2e-7f
#define RNMX (1.f - EPS)

float ran1(long);
float ran1(long *);

float ran1(long start = 0)
{
    static long init;
    if (start)
        init = start;
    return ran1(&init);
}

/**
 * ?Minimal? random number generator of Park and Miller with Bays-Durham shuffle
and added
safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of
the endpoint
values). Call with idum a negative integer to initialize; thereafter, do not
alter idum between
successive deviates in a sequence. RNMX should approximate the largest floating
value that is
less than 1.
 * @param idum
 * @return
 */
float ran1(long *idum)
{
    int j;
    long k;
    static long iy = 0;
    static long iv[NTAB];
    float temp;
    if (*idum <= 0 || !iy) {
        if (-(*idum) < 1)
            *idum = 1;
        else
            *idum = -(*idum);
        for (j = NTAB + 7; j >= 0; j--) {
            k = (*idum) / IQ;
            *idum = IA * (*idum - k * IQ) - IR * k;
            if (*idum < 0)
                *idum += IM;
            if (j < NTAB)
                iv[j] = *idum;
        }
        iy = iv[0];
    }
    k = (*idum) / IQ;

    *idum = IA * (*idum - k * IQ) - IR * k;
    *idum += IM;
    j = iy / NDIV;

    iy = iv[j];
    iv[j] = *idum;
    if ((temp = AM * iy) > RNMX)
        return RNMX;
    else
        return temp;
}

#endif //#ifndef _APEX_SRC_LIB_APEXTOOLS_APEXRANDOM_H_
