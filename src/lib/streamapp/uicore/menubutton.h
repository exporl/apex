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
 
#ifndef __MENUBUTTON_H__
#define __MENUBUTTON_H__

#include "button.h"
#include "component.h"

namespace uicore
{

  class Screen;

    /**
      * MenuButton
      *   simple button for use in a menu.
      *   Size is fixed to 120 * 40,
      *   color to nice blue.
      *   @see Button
      *   @see Menu
      *********************************** */
  class MenuButton : public Button
  {
  public:
      /**
        * Constructor.
        * @param a_pParent the parent Screen
        * @param ac_pText the button text
        * @param a_pTarget the target for action commands
        */
    MenuButton( Screen* a_pParent, const char* ac_pText = 0, MenuActionDispatcher* a_pTarget = 0 ) :
      Button( a_pParent )
    {
      ActionComponent::mp_InstallEventDispatcher( a_pTarget );
      Button::mp_SetSize( 120, 40 );
      Button::mp_SetBorder( 5 );
      Button::mp_SetBaseColor( 0x22, 0x22, 0x77 );
      Button::mp_SetText( ac_pText );
      Button::mp_Show( true );
    }

      /**
        * Destructor.
        */
    ~MenuButton()
    {
    }
  };

}

#endif //#ifndef __MENUBUTTON_H__
