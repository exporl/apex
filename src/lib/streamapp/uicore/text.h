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
 
#ifndef __UICORETEXT_H__
#define __UICORETEXT_H__

#include "component.h"

namespace uicore
{

  class Font;

    /**
      * Button
      *   simple button class.
      *   Renders a (quite fancy) 3D look button with text.
      ***************************************************** */
  class TextComponent : public Component
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parent Screen, must be non-zero
        */
    TextComponent( Screen* a_pParent, const char* ac_psText = 0 );

      /**
        * Destructor.
        */
    virtual ~TextComponent();

      /**
        * Implementation of the Component method.
        */
    virtual void mf_Render();

      /**
        * Set to clear the text area first.
        * This is needed when using a TextComponent as is,
        * and not erasing the screen first, since unlike other
        * components, text does not fill the same area over and over again.
        * Don't use it for buttons etc, they should be smart enough to render
        * their background first. followed by the text.
        * @param ac_bSet true to erase the entire rect first.
        */
    INLINE void mp_SetEraseFirst( const bool ac_bSet )
    {
      mv_bEraseFirst = ac_bSet;
    }

      /**
        * Set the text to display.
        * Must fit into the button, else
        * it will not be rendered.
        * @param ac_psText text string
        */
    virtual void mp_SetText( const char* ac_psText );

      /**
        * Set the new Font to use for the text.
        * @param a_Font the new font.
        */
    virtual void mp_SetFont( const Font& a_Font );

      /**
        * Get the current Font.
        * @return a mutable reference
        */
    virtual Font& mp_GetFont();

    static const unsigned sc_nMaxTextLen = 256;

  protected:
    virtual void mf_RenderText( Painter* a_pPainter );

    Font*     m_pFont;
    bool      mv_bEraseFirst;
    char      mv_psText[ sc_nMaxTextLen ];
  };

}

#endif //#ifndef __UICORETEXT_H__
