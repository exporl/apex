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
 
#ifndef __TABLE_H__
#define __TABLE_H__

#include "component.h"
#include "containers/ownedarray.h"
using namespace streamapp;

namespace uicore
{
    /**
      * Table
      *   simple table layout.
      *   A table has a number of cells, divided in rows and columns,
      *   each with the same width and height.
      *   Each cell can, but is not required to, contain a Component.
      *   The only functionality a Table has, is positioning the
      *   Components added; excapt when removing an item: then it will
      *   be deleted.
      *   @see Menu for a Table with more functionality.
      *************************************************************** */
  class Table
  {
  public:
      /**
        * Constructor.
        * All values in pixels.
        * @param ac_nX the x position
        * @param ac_nY the y position
        * @param ac_nColW the width of a column
        * @param ac_nColH the height of a column
        */
    Table( const unsigned ac_nX = 50, const unsigned ac_nY = 50, const unsigned ac_nColW = 140, const unsigned ac_nColH = 50 );

      /**
        * Destructor.
        */
    virtual ~Table();

      /**
        * A single column
        */
    typedef DynamicArray<Component*> mt_ColumnItems;

      /**
        * An array of columns == a grid.
        */
    typedef OwnedArray<mt_ColumnItems> mt_GridItems;

      /**
        * Add an IMenuItem after the previous one.
        * @param a_pItem the item, must be non-zero
        * @param ac_nColumn the column to add it to.
        */
    virtual void mp_AddItem( Component* a_pItem, const unsigned ac_nColumn = 0 );

      /**
        * Set a cell to contain the new Component given.
        * The cell must already exist, that is, added by mp_addItem().
        * @param a_pItem new Component
        *@ param ac_nRow the row to set
        * @param ac_nColumn the column to set
        */
    virtual void mp_SetItem( Component* a_pItem, const unsigned ac_nRow, const unsigned ac_nColumn = 0 );

      /**
        * Remove an item.
        * Removes the item at the given position, shifting the other items down, if any.
        * @param ac_nRow the row to remove the item from
        * @param ac_nColumn the column the column to remove the item from
        */
    virtual void mp_RemoveItem( const unsigned ac_nRow, const unsigned ac_nColumn = 0 );

  protected:
    virtual void mp_PositionItem( Component* a_pItem, const unsigned ac_nRow, const unsigned ac_nColumn );

    const unsigned  mc_nX;
    const unsigned  mc_nY;
    const unsigned  mc_nW;
    const unsigned  mc_nH;
    mt_GridItems    m_Items;
  };

}

#endif //#ifndef __TABLE_H__
