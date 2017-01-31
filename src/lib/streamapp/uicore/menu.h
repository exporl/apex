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
 
#ifndef __MENU_H__
#define __MENU_H__

#include "table.h"
#include "containers/ownedarray.h"
using namespace streamapp;

namespace uicore
{
    /**
      * Menu
      *   simple grid menu implementation.
      *   Use for navigating a menu of Components.
      *   The Component that is currently selected will be
      *   highlighted, while all others are just shown.
      *   Cycling always stays in the same row or column,
      *   eg if the end of column 1 is reached, it goes back
      *   to the start of column 1, not the start of column 2.
      *   Default IMenuItems (ie without a Component) are
      *   treated as spacers and are skipped when cycling,
      *   so if you want a spacer with a content you should
      *   override it's mf_pGetComponent() to return 0, even
      *   if the Screen draws it.
      *   Everything that goes out of bounds is simply ignored,
      *   and a debug message is printed.
      *   @see mf_Select()
      *   @see Component
      ********************************************************* */
  class Menu : private Table
  {
  public:
      /**
        * Constructor.
        * All values in pixels.
        * @param ac_nID a unique ID
        * @param ac_nX the x position
        * @param ac_nY the y position
        * @param ac_nColW the width of a column
        * @param ac_nColH the height of a column
        */
    Menu( const unsigned ac_nID, const unsigned ac_nX = 50, const unsigned ac_nY = 50, const unsigned ac_nColW = 140, const unsigned ac_nColH = 50 );

      /**
        * Destructor.
        */
    ~Menu();

      /**
        * Add an IMenuItem after the previous one.
        * @param a_pItem the item, must be non-zero
        * @param ac_nColumn the column to add it to.
        */
    void mp_AddMenuItem( Component* a_pItem, const unsigned ac_nColumn = 0 );

      /**
        * This calls mp_DoAction() on the currently selected IMenuItem.
        * Never called for items that have no Component.
        * If called before navigating, selects the item at ( 0, 0 ).
        */
    void mf_Select();

      /**
        * Navigate up.
        */
    void mf_Up();

      /**
        * Navigate down.
        */
    void mf_Down();

      /**
        * Navigate left.
        */
    void mf_Left();

      /**
        * Navigate right.
        */
    void mf_Right();

      /**
        * Navigate to the given position.
        * Nothing happens if not in range.
        * @param ac_nPosition the y position
        * @param ac_nColumn the x position
        */
    void mf_SetPosition( const unsigned ac_nPosition, const unsigned ac_nColumn = 0 );

  private:
    const unsigned  mc_nID;
    int             mv_nPosition;
    int             mv_nColumn;
  };

}

#endif //#ifndef __MENU_H__
