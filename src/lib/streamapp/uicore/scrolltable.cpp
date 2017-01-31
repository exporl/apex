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
 
#include "scrolltable.h"

using namespace uicore;


ScrollTable::ScrollTable(const unsigned int ac_nMaxNumItems, const unsigned ac_nX, const unsigned ac_nY, const unsigned ac_nColW, const unsigned ac_nColH ) :
    Table( ac_nX, ac_nY, ac_nColW, ac_nColH ),
  mc_nMaxNumItems( ac_nMaxNumItems )
{
}

ScrollTable::~ScrollTable()
{
}

void ScrollTable::mp_AddItem( Component *a_pItem, const unsigned int ac_nColumn )
{
  Table::mp_AddItem( a_pItem, ac_nColumn );
  mt_ColumnItems* pCur = m_Items( ac_nColumn );
  if( pCur->mf_nGetNumItems() > mc_nMaxNumItems )
    Table::mp_RemoveItem( 0, ac_nColumn );
}
