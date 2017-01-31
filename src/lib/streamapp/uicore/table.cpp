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
 
#include "table.h"
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

using namespace uicore;

Table::Table( const unsigned ac_nX, const unsigned ac_nY, const unsigned ac_nColW, const unsigned ac_nColH ) :
  mc_nX( ac_nX ),
  mc_nY( ac_nY ),
  mc_nW( ac_nColW ),
  mc_nH( ac_nColH )
{
    //insert one column already
  m_Items.mp_AddItem( new mt_ColumnItems() );
}

Table::~Table()
{
}

void Table::mp_PositionItem( Component* a_pItem, const unsigned ac_nRow, const unsigned ac_nColumn )
{
  a_pItem->mp_SetPosition( mc_nX + mc_nW * ac_nColumn, mc_nY + mc_nH * ac_nRow );
  DBGPF( "xpos = %d, ypos = %d", mc_nX + mc_nW * ac_nColumn, mc_nY + mc_nH * ac_nRow );
}

void Table::mp_AddItem( Component* a_pItem, const unsigned ac_nColumn )
{
    //see if there are enough columns
  const unsigned nColumns = m_Items.mf_nGetNumItems();
  const unsigned nColPlusOne = ac_nColumn + 1;
  if( nColPlusOne > nColumns )
  {
    DBGPF( "setting %d columns", nColPlusOne );
    for( unsigned i = nColumns ; i < nColPlusOne ; ++i )
      m_Items.mp_AddItem( new mt_ColumnItems() );
  }

    //set the item's position
  if( a_pItem )
    mp_PositionItem( a_pItem, m_Items( ac_nColumn )->mf_nGetNumItems(), ac_nColumn );

    //add
  m_Items( ac_nColumn )->mp_AddItem( a_pItem );
}

void Table::mp_SetItem( Component* a_pItem, const unsigned ac_nRow, const unsigned ac_nColumn )
{
  const unsigned nColumns = m_Items.mf_nGetNumItems();
  if( ac_nColumn > nColumns )
    return;
  mt_ColumnItems* pCur = m_Items( ac_nColumn );
  const unsigned nItems = pCur->mf_nGetNumItems();
  if( ac_nRow > nItems )
    return;
  pCur->mp_SetItem( ac_nRow, a_pItem );
}

void Table::mp_RemoveItem( const unsigned ac_nRow, const unsigned ac_nColumn )
{
  const unsigned nColumns = m_Items.mf_nGetNumItems();
  if( ac_nColumn > nColumns )
    return;
  mt_ColumnItems* pCur = m_Items( ac_nColumn );
  const unsigned nItems = pCur->mf_nGetNumItems();
  if( ac_nRow > nItems )
    return;
  pCur->mp_RemoveItemAt( ac_nRow );
  const unsigned nItMinusOne = nItems - 1;
  for( unsigned i = 0 ; i < nItMinusOne ; ++i )
    mp_PositionItem( pCur->operator ()( i ), i, ac_nColumn );
}
