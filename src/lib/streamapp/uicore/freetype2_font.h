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
 
#ifndef __FREETYPE2_FONT_H__
#define __FREETYPE2_FONT_H__

#include "font.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace uicore
{

  class FreeType2Font : public Font
  {
  public:
      /**
        * Constructor.
        */
    FreeType2Font();

      /**
        * Destructor.
        * Calls mp_UnInitialize().
        */
    ~FreeType2Font();

      /**
        * Implementation of the Font method.
        * @param ac_sFontName path to the ttf file to use
        */
    bool mp_bInitialize( const char* ac_sFontName );

      /**
        * Implementation of the Font method.
        */
    void mp_UnInitialize();

      /**
        * Implementation of the Font method.
        */
    bool mf_bTextSize( const char* ac_psText, const unsigned ac_nPixelHeight,
                       unsigned& a_nTextWidth, unsigned& ac_nTextHeight );

      /**
        * Implementation of the Font method.
        */
    bool mf_bRenderText( const unsigned ac_nX, const unsigned ac_nY, unsigned short* a_pDest, const char* ac_psText );

  private:
    static const unsigned sc_nConvertSize = 256;

    void mp_InitColor( const tRgb& ac_Color = tRgb::sc_White() );

    bool        mv_bInit;
    FT_Library  m_Lib;
    FT_Face     m_Face;
    u8          m_8bit5[ sc_nConvertSize ];
    u8          m_8bit6[ sc_nConvertSize ];
  };

}

#endif //#ifndef __FREETYPE2_FONT_H__
