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
 
#include "gfx.h"

unsigned short uicore::tRgb::mf_ColorConvert() const
{
    //Scale the color values to a 565 16bit image
  u8 rc = r * ( 1 << 5) / ( 1 << 8 );
  u8 gc = g * ( 1 << 6) / ( 1 << 8 );
  u8 bc = b * ( 1 << 5) / ( 1 << 8 );

  return ( bc | ( gc << 5 ) | ( rc << 11 ) );
}

unsigned short uicore::tRgb::sf_ColorConvert( const u8 ac_nRed, const u8 ac_nGreen, const u8 ac_nBlue )
{
    //Scale the color values to a 565 16bit image
  u8 rc = ac_nRed * ( 1 << 5) / ( 1 << 8 );
  u8 gc = ac_nGreen * ( 1 << 6) / ( 1 << 8 );
  u8 bc = ac_nBlue * ( 1 << 5) / ( 1 << 8 );

  return ( bc | ( gc << 5 ) | ( rc << 11 ) );
}

uicore::tRgb uicore::tRgb::sf_ColorConvert( const unsigned short ac_nColor )
{
  tRgb ret;

  ret.r = (u8) ( ac_nColor >> 11 )* 0x100 / 0x20;
  ret.g = (u8) ( ac_nColor >> 5 ) * 0x100 / 0x40;
  ret.b = (u8) ( ac_nColor ) * 0x100 / 0x20;

  return ret;
}
