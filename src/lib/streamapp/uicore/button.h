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
 
#ifndef __UICOREBUTTON_H__
#define __UICOREBUTTON_H__

#include "text.h"

namespace uicore
{

    /**
      * Button
      *   simple button class.
      *   Renders a (quite fancy) 3D look button with text.
      ***************************************************** */
  class Button : public TextComponent
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parent Screen, must be non-zero
        */
    Button( Screen* a_pParent );

      /**
        * Destructor.
        */
    virtual ~Button();

      /**
        * Implementation of the Component method.
        */
    void mf_Render();

      /**
        * Set the width of the border.
        * @param ac_nW the width in pixels
        */
    void mp_SetBorder( const unsigned ac_nW );

      /**
        * Set flat button.
        * @param ac_bSet true to discard 3D look
        */
    void mp_SetFlat( const bool ac_bSet );

      /**
        * Set the base color.
        * @param ac_nR red value
        * @param ac_nG green value
        * @param ac_nB blue value
        */
    void mp_SetBaseColor( const u8 ac_nR, const u8 ac_nG, const u8 ac_nB );

      /**
        * Set the base color.
        * @param ac_Color the color
        */
    void mp_SetBaseColor( const tRgb& ac_Color );

  protected:
    void mf_RenderButton( Painter* a_pPainter, const u8 ac_nR, const u8 ac_nG, const u8 ac_nB );

    tRgb      mv_BaseColor;
    bool      mv_bFlat;
    unsigned  mv_nBorder;
  };

}

#endif //#ifndef __UICOREBUTTON_H__
