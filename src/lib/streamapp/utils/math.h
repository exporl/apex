/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/
 
#ifndef __STREAMAPPMATH_H__
#define __STREAMAPPMATH_H__

#include "defines.h"
#include <math.h>

  /**
    * namespace for mathematical stuff
    ********************************** */
namespace math
{
    /**
      * Pi
      */
  const double gc_dPi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823;
  const double gc_d2Pi = 2.0 * gc_dPi;

    /**
      * Square root of 2
      */
  const double gc_dSqrt2 = pow( 2.0, 0.5 );

    /**
      * Phi
      */
  const double gc_dPhi  = 1.6180339887;
  const double gc_dPhiR = 0.6180339887;

    /**
      * Degrees.
      */
  const double gc_d90  = 90.0;
  const double gc_d180 = 180.0;
  const double gc_d360 = 360.0;

    /**
      * Round a number.
      */
  INLINE double f_dRound( const double ac_dToRound )
  {
    return floor( ac_dToRound + 0.5 );
  }

    /**
      * Convert degrees to radians.
      */
  INLINE double f_dDegreesToRadians( const double ac_dDegrees )
  {
    return( ac_dDegrees * gc_dPi / gc_d180 );
  }

    /**
      * Convert radians to degrees.
      */
  INLINE double f_dDRadiansToDegrees( const double ac_dRadians )
  {
    return( ac_dRadians * gc_d180 / gc_dPi );
  }

    /**
      * Calculate qutient and remainder
      */
  INLINE void gf_QuotRem( int numerator, int denominator, int* quotient, int* remainder )
  {
     *quotient  = numerator / denominator;
     *remainder = denominator % numerator;
  }

  INLINE void gf_QuotRem( unsigned numerator, unsigned denominator, unsigned* quotient, unsigned* remainder )
  {
     *quotient  = numerator / denominator;
     *remainder = denominator % numerator;
  }

    /**
      * Round to the nearest multiple.
      * eg for 5 and 16, true -> 16
      * for 20 and 16, false -> 16
      * @param ac_nToRound the number to round
      * @param ac_nMultiple the multiple to round to
      * @param ac_bUp true to round up, else down
      * @return the nearest multiple
      */
  template< class tType >
  tType gf_RoundToMultiple( const tType ac_nToRound, const tType ac_nMultiple, const bool ac_bUp )
  {
    //s_static_assert( types::tIsInteger< type > );
    Q_ASSERT( ac_nMultiple != 0 && "can't divide by zero!!" );
    tType q = ac_nToRound / ac_nMultiple;
    tType r = ac_nToRound - ( q * ac_nMultiple );
    q = ac_bUp ? q + 1 : q;
    return ( r == 0 ) ? ac_nToRound : q * ac_nMultiple;
  }

}

#endif //#ifndef __STREAMAPPMATH_H__
