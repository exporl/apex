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
 
#ifndef __GFX_H__
#define __GFX_H__

#include "defines.h"

namespace uicore
{
  #ifndef u8
  #define u8 unsigned char       //FIXME check types
  #endif

    /**
      * tRgb
      *   RGB color container.
      *   FIXME check alignment, should be 8 bytes,
      *   else it's no use regarding memory space.
      ********************************************* */
  struct tRgb
  {
      /**
        * Default Constructor.
        * Black.
        */
    tRgb() :
      r( 0 ), g( 0 ), b( 0 )
    {
    }

      /**
        * Constructor.
        * @param ac_nRed red value
        * @param ac_nGreen green value
        * @param ac_nBlue blue value
        */
    tRgb( const u8 ac_nRed, const u8 ac_nGreen, const u8 ac_nBlue ) :
      r( ac_nRed ), g( ac_nGreen ), b( ac_nBlue )
    {
    }

      /**
        * Set the values.
        * @param ac_nRed red value
        * @param ac_nGreen green value
        * @param ac_nBlue blue value
        */
    INLINE void mp_Set( const u8 ac_nRed, const u8 ac_nGreen, const u8 ac_nBlue )
    {
      r = ac_nRed;
      g = ac_nGreen;
      b = ac_nBlue;
    }

      /**
        * Scale the color values to a 565 16bit image.
        * @return an unsigned short pixel value
        */
    unsigned short mf_ColorConvert() const;

      /**
        * Scale the color values to a 565 16bit image.
        * Static for convenience.
        * @param ac_nRed  red value
        * @param ac_nGreen  green value
        * @param ac_nBlue  blue value
        * @return an unsigned short pixel value
        */
    static unsigned short sf_ColorConvert( const u8 ac_nRed, const u8 ac_nGreen, const u8 ac_nBlue );

      /**
        * Inverse of the above method.
        * @param ac_nColor the color
        * @return the tRgb for it.
        */
    static tRgb sf_ColorConvert( const unsigned short ac_nColor );

      /**
        * Get the color black.
        * @return reference
        */
    INLINE static const uicore::tRgb& sc_Black()
    {
      static uicore::tRgb ret;
      return ret;
    }

      /**
        * Get the color white.
        * @return reference
        */
    INLINE static const tRgb& sc_White()
    {
      static uicore::tRgb ret( 255, 255, 255 );
      return ret;
    }

    u8 r;
    u8 g;
    u8 b;
  };

    /**
      * tPoint
      *   point container.
      ******************** */
  struct tPoint
  {
      /**
        * Default Constructor.
        * Initializes to ( 0, 0 )
        */
    tPoint() :
      x( 0 ), y( 0 )
    {
    }

      /**
        * Constructor.
        * @param ac_nX the x position
        * @param ac_nY the y position
        */
    tPoint( const unsigned ac_nX, const unsigned ac_nY ) :
      x( ac_nX ), y( ac_nY )
    {
    }

    unsigned x;
    unsigned y;
  };

    /**
      * tRect
      *   rectangle container.
      ************************ */
  struct tRect
  {
      /**
        * Default Constructor.
        * Creates empty rect.
        */
    tRect() :
      x( 0 ), y( 0 ), w( 0 ), h( 0 )
    {
    }

      /**
        * Constructor.
        * All values in pixels.
        * @param ac_nX x offset
        * @param ac_nY y offset
        * @param ac_nW width
        * @param ac_nH height
        */
    tRect( const unsigned ac_nX, const unsigned ac_nY, const unsigned ac_nW, const unsigned ac_nH ) :
      x( ac_nX ), y( ac_nY ), w( ac_nW ), h( ac_nH )
    {
    }

    unsigned x;
    unsigned y;
    unsigned w;
    unsigned h;
  };

}

#endif //#ifndef __GFX_H__
