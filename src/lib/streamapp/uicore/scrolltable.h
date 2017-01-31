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
 
#ifndef __SCRLLTABLE_H__
#define __SCRLLTABLE_H__

#include "table.h"

namespace uicore
{

    /**
      * ScrollTable
      *   a Table with scrolling functionality.
      ***************************************** */
  class ScrollTable : public Table
  {
  public:
      /**
        * Constructor.
        * @param ac_nMaxNumItems if the number of items gets bigger then this number, the table scrolls down
        * @see Table::Table()
        */
    ScrollTable( const unsigned ac_nMaxNumItems, const unsigned ac_nX = 50, const unsigned ac_nY = 50, const unsigned ac_nColW = 140, const unsigned ac_nColH = 50 );

      /**
        * Destructor.
        */
    ~ScrollTable();

      /**
        * Implementation of the Table method.
        */
    void mp_AddItem( Component* a_pItem, const unsigned ac_nColumn = 0 );

  private:
    const unsigned mc_nMaxNumItems;
  };

}

#endif //#ifndef __SCRLLTABLE_H__
