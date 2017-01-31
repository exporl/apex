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
 
#ifndef __FONT_H__
#define __FONT_H__

#include "gfx.h"

namespace uicore
{

    /**
      * Font
      *   interface for defining fonts.
      *   Has the most basic methods only for now, namely
      *   the rendering and getting the size.
      *   Implemented in platform-specific code, windows/linux
      *   can use FreeType2, others use pixmap based fonts.
      *   @see Text for a class that renders text.
      ******************************************************************** */
  class Font
  {
  protected:
      /**
        * Protected Constructor.
        */
    Font() :
      mv_nScrW( 0 ),
      mv_nScrH( 0 ),
      mv_nSize( 0 ),
      mv_nFade( 255 ),
      mv_bBold( false ),
      mv_Color( tRgb::sc_White() )
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~Font()
    {
    }

      /**
        * Initialize the font.
        * This must setup all internals, like initting
        * the library etc.
        * Shouldn't do anything if already initialized,
        * except when ac_sFontName is different, then it should
        * mp_UnInitialize() and re-initialize.
        * @param ac_sFontName the name of the font to use. Can
        * be a path to a file, or just a name if the font is embedded.
        * @return true if succeeded
        */
    virtual bool mp_bInitialize( const char* ac_sFontName ) = 0;

      /**
        * Uninitialize the font.
        * Deletes created resources.
        * This yields all other methods unusable.
        */
    virtual void mp_UnInitialize() = 0;

      /**
        * Get the size of the given string on screen,
        * using the current font set.
        * Returns 0 when not initialized.
        * @param ac_psText the string to get the sizes for
        * @param ac_nPixelHeight the height (pointsize) of the font to use
        * @param a_nTextWidth receives the width in pixels
        * @param a_nTextHeight receives the height in pixels
        * @return false if an error occurred
        */
    virtual bool mf_bTextSize( const char* ac_psText, const unsigned ac_nPixelHeight,
                               unsigned& a_nTextWidth, unsigned& ac_nTextHeight ) = 0;

      /**
        * Set the font size to use.
        * @param ac_nSize the size in points.
        */
    INLINE  void mp_SetFontSize( const unsigned ac_nSize )
    {
      mv_nSize = ac_nSize;
    }

      /**
        * Set bold font.
        * @param ac_bSet true for bold
        */
    INLINE  void mp_SetBold( const bool ac_bSet )
    {
      mv_bBold = ac_bSet;
    }

      /**
        * Set the height to clip at.
        * @param ac_nHeight.height in pixels
        */
    INLINE  void mp_SetScreenHeight( const unsigned ac_nHeight )
    {
      mv_nScrH = ac_nHeight;
    }

      /**
        * Set the width to clip at.
        * @param ac_nWidth.widtt in pixels
        */
    INLINE void mp_SetScreenWidth( const unsigned ac_nWidth )
    {
      mv_nScrW = ac_nWidth;
    }

      /**
        * Set the text color.
        * Deafults to white.
        * @param ac_Color the color
        */
    INLINE void mp_setColor( const tRgb& ac_Color )
    {
      mv_Color = ac_Color;
    }

      /**
        * Set the fade factor for the color.
        * Deafults to 255.
        * @param ac_Color the color
        */
    INLINE void mp_SetFadeFactor( const u8 ac_nFade )
    {
      mv_nFade = ac_nFade;
    }

    INLINE const unsigned& mf_nGetFontSize() const
    {
      return mv_nSize;
    }

      /**
        * Renders text.
        * This renders the pixels needed to display the text,
        * starting at the given position in the destination buffer.
        * @param ac_nX the x position
        * @param ac_nY the y position
        * @param a_pDest the destination pixel buffer
        * @param ac_psText pointer to a null-terminated string with the text
        * @return false if an error occurred
        */
    virtual bool mf_bRenderText( const unsigned ac_nX, const unsigned ac_nY, unsigned short* a_pDest, const char* ac_psText ) = 0;

  protected:
    unsigned  mv_nScrW;
    unsigned  mv_nScrH;
    unsigned  mv_nSize;
    u8        mv_nFade;
    bool      mv_bBold;
    tRgb      mv_Color;
  };

}

#endif //#ifndef __FONT_H__
