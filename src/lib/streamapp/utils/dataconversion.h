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

#ifndef dataconversionS_H
#define dataconversionS_H

#include "../defines.h"
#include "../typedefs.h"

#include <math.h>

/**
  * namespace with dataconversion tools
  ************************************* */
namespace dataconversion
{

/**
  * Convert from dB (log) to linear scale ( 0...1 ).
  * Everything below sc_dMixerMin is mapped 0.0.
  * @param ac_dGainIndB number to convert
  * @return number in linear scale
  */
INLINE double gf_dBtoLinear(const double ac_dGainIndB)
{
    return (ac_dGainIndB <= streamapp::sc_dMixerMin)
               ? 0.0
               : pow(10.0, ac_dGainIndB / 20.0);
}

/**
  * Convert from linear scale ( 0...1 ) to dB (log).
  * 0.0 is mapped to sc_dMixerMin.
  * @param ac_dGainInLin number to convert
  * @return number in dB
  */
INLINE double gf_LinearTodB(const double ac_dGainInLin)
{
    return (ac_dGainInLin <= 0.0) ? streamapp::sc_dMixerMin
                                  : (20.0 * log10(ac_dGainInLin));
}

/**
  * Convert 32bit to 64bit float.
  * @param n to convert
  * @return converted
  */
INLINE double ToDouble(const float n)
{
    return (double)n;
}

/**
  * Convert 32bit int to 64bit float.
  * @param n to convert
  * @return converted
  */
INLINE double ToDouble(const int n)
{
    return (double)n;
}

/**
  * Convert 64bit to 32bit float.
  * This causes a loss of precision.
  * FIXME is standard cast sufficient?
  * @param n to convert
  * @return converted
  */
INLINE float ToFloat(const double n)
{
    return (float)n;
}

/**
  * CLip a double to +/- 1.0.
  * Nothing happens if the double is in range,
  * else it's set to 32bit max.
  * @param a the double to check
  * @return true if a was replaced with +/- 1.0
  */
INLINE bool f_dClipNormalized(double &a)
{
    if (a > 1.0) {
        a = 1.0;
        return true;
    } else if (a < -1.0) {
        a = -1.0;
        return true;
    }
    return false;
}

/**
  * CLip a double to 32 bit.
  * Nothing happens if the double is in range,
  * else it's set to 32bit max.
  * @param a the double to check
  * @return true if a was replaced with 32bit max
  */
INLINE bool f_dClip32bit(double *a)
{
    if (*a > streamapp::sc_d32BitMinMax) {
        *a = streamapp::sc_d32BitMinMax;
        return true;
    } else if (*a < -streamapp::sc_d32BitMinMax) {
        *a = -streamapp::sc_d32BitMinMax;
        return true;
    }
    return false;
}

/**
  * Convert double to int.
  * This is about 6% faster than using the normal c++ cast to convert a double
 * to an int, and
  * it will round the value to the nearest integer, instead of rounding it down
 * like the normal cast does.
  * The constant used is 6755399441055744 = 2^52 + 2^51. The method is machine
 * dependent as it relies on the
  * binary representation of the floating point mantissa. Here it is assumed
 * that, firstly, the floating point
  * number has a 53-bit mantissa with the most significant bit (that is always
 * one with normalized numbers)
  * omitted, and secondly, the address of the number points to the mantissa.
  */
INLINE int roundDoubleToInt(const double n) throw()
{
    /*double dRet = n;
    dRet += 6755399441055744.0;
    return *((int*)&dRet);*/
    return (int)n; // [Tom] FIXME
}

template <class tIntegerType>
INLINE tIntegerType roundDoubleToIntT(const double n) throw()
{
    /*double dRet = n;
    dRet += 6755399441055744.0;
    return *((tIntegerType*)&dRet);*/
    return (int)n; // [Tom] FIXME
}

/**
  * Converts 3 little-endian bytes into a signed 24-bit value (which is
 * sign-extended to 32 bits).
  * @param bytes the memory containing 3 bytes
  * @return the contained value
  */
INLINE int littleEndian24Bit(const char *const bytes) throw()
{
    return (((int)bytes[2]) << 16) |
           (((unsigned int)(unsigned char)bytes[1]) << 8) |
           ((unsigned int)(unsigned char)bytes[0]);
}

/**
  * Converts 3 big-endian bytes into a signed 24-bit value (which is
 * sign-extended to 32 bits).
  * @param bytes the memory containing 3 bytes
  * @return the contained value
  */
INLINE int bigEndian24Bit(const char *const bytes) throw()
{
    return (((int)bytes[0]) << 16) |
           (((unsigned int)(unsigned char)bytes[1]) << 8) |
           ((unsigned int)(unsigned char)bytes[2]);
}

/**
  * Copies a 24-bit number to 3 little-endian bytes.
  * @param value the number
  * @param destBytes the memory to copy t
  */
INLINE void littleEndian24BitToChars(const int value,
                                     char *const destBytes) throw()
{
    destBytes[0] = (char)(value & 0xff);
    destBytes[1] = (char)((value >> 8) & 0xff);
    destBytes[2] = (char)((value >> 16) & 0xff);
}

/**
  * Copies a 24-bit number to 3 big-endian bytes.
  * @param value the number
  * @param destBytes the memory to copy to
  */
INLINE void bigEndian24BitToChars(const int value,
                                  char *const destBytes) throw()
{
    destBytes[0] = (char)((value >> 16) & 0xff);
    destBytes[1] = (char)((value >> 8) & 0xff);
    destBytes[2] = (char)(value & 0xff);
}

/**
  * Calculate number of samples from time, based on samplerate.
  * @param ac_dFs the samplerate
  * @param ac_dMilliSeconds the time
  * @return number of samples
  */
template <class tIntegerType>
INLINE tIntegerType gf_nSamplesFromMsec(const double ac_dFs,
                                        const double ac_dMilliSeconds)
{
    const double n = (ac_dMilliSeconds / 1000.0) * ac_dFs;
    return roundDoubleToIntT<tIntegerType>(n);
}
}

#endif //#ifndef dataconversionS_H
