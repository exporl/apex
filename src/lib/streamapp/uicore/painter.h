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
 
#ifndef __PAINTER_H__
#define __PAINTER_H__

#include "gfx.h"

namespace uicore
{

  class Font;
  class Screen;

    /**
      * Painter
      *   class responsible for drawing on a Screen.
      *   It's ok to allocate a lot of painters since they
      *   only contain one pointer. This is nicer then
      *   passing the same painter around all over the place.
      ******************************************************* */
  class Painter
  {
  public:
      /**
        * Constructor.
        * @param a_pScreen the screen to draw on
        */
    Painter( Screen* const ac_pScreen );

      /**
        * Copy Constructor.
        * @param ac_Rh the instance to copy
        */
    Painter( const Painter& ac_Rh );

      /**
        * Destructor.
        */
    ~Painter();

      /**
        * Clear the entire screen.
        * @param ac_Color the erase color
        */
    void mf_Clear( const tRgb& ac_Color = tRgb::sc_Black() );

      /**
        * Clear a specific area.
        * Always safe to call, clips the values.
        * All values in pixels.
        * @param ac_nX x offset
        * @param ac_nY y offset
        * @param ac_nW width
        * @param ac_nH height
        * @param ac_Color the erase color
        */
    void mf_Clear( const unsigned ac_nX,
                   const unsigned ac_nY,
                   const unsigned ac_nW,
                   const unsigned ac_nH,
                   const tRgb& ac_Color = tRgb::sc_Black() );

      /**
        * Clear a specific area.
        * Always safe to call, clips the values.
        * All values in pixels.
        * @param ac_Area the area
        * @param ac_Color  the erase color
        */
    void mf_Clear( const tRect& ac_Area, const tRgb& ac_Color = tRgb::sc_Black() );

    void mf_FillRectangle( const tRect& ac_Rectangle, unsigned short col );

    void mf_FillRectangle( const unsigned x, const unsigned y, const unsigned w, const unsigned h, unsigned short col );

    void mf_DrawRectangle( unsigned x, unsigned y, unsigned w, unsigned h, const u8 r, const u8 g, const u8 b );

    void mf_FillTriangle( unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned x3, unsigned y3, unsigned short col );

    void mf_DrawTriangle( unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned x3, unsigned y3, const u8 r, const u8 g, const u8 b );

    void mf_DrawText( const unsigned ac_nX, const unsigned ac_nY, const char* ac_psText, Font& a_Font );

  protected:
    Screen* const   mc_pScreen;

    Painter& operator = ( const Painter& );
  };

}

#endif //#ifndef __PAINTER_H__
