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
 
#include "menu.h"
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

using namespace uicore;

Menu::Menu( const unsigned ac_nID, const unsigned ac_nX, const unsigned ac_nY, const unsigned ac_nColW, const unsigned ac_nColH ) :
    Table( ac_nX, ac_nY, ac_nColW, ac_nColH ),
  mc_nID( ac_nID ),
  mv_nPosition( 0 ),
  mv_nColumn( 0 )
{
}

Menu::~Menu()
{
}

void Menu::mp_AddMenuItem( Component* a_pItem, const unsigned ac_nColumn )
{
  Table::mp_AddItem( a_pItem, ac_nColumn );
}

void Menu::mf_Up()
{
  mt_ColumnItems* pCur = m_Items( mv_nColumn );
  const unsigned nItems = pCur->mf_nGetNumItems();
  if( nItems == 0 )
    return;

  Component* p = pCur->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( false );
up:
  mv_nPosition = ( mv_nPosition + 1 ) % nItems;

  p = pCur->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( true );
  else
    goto up; //skip on spacer
}

void Menu::mf_Down()
{
  mt_ColumnItems* pCur = m_Items( mv_nColumn );
  const unsigned nItems = pCur->mf_nGetNumItems();
  if( nItems == 0 )
    return;

  Component* p = pCur->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( false );
down:
  mv_nPosition--;
  if( mv_nPosition < 0 )
    mv_nPosition = nItems - 1;

  p = pCur->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( true );
  else
    goto down;
}

void Menu::mf_Left()
{
  mt_ColumnItems* pCur = m_Items( mv_nColumn );
  Component* p = pCur->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( false );

left:
  mv_nColumn--;
  if( mv_nColumn < 0 )
    mv_nColumn = m_Items.mf_nGetNumItems() - 1;

  pCur = m_Items( mv_nColumn );

  const unsigned nCurColItems = pCur->mf_nGetNumItems();
  if( nCurColItems == 0 )
    goto left;

  mv_nPosition = s_min( mv_nPosition, (int) ( nCurColItems - 1 ) );

  p = m_Items( mv_nColumn )->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( true );
  else
    mf_Up();
}

void Menu::mf_Right()
{
  mt_ColumnItems* pCur = m_Items( mv_nColumn );
  Component* p = pCur->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( false );

right:
  mv_nColumn = ( mv_nColumn + 1 ) % m_Items.mf_nGetNumItems();

  pCur = m_Items( mv_nColumn );

  const unsigned nCurColItems = pCur->mf_nGetNumItems();
  if( nCurColItems == 0 )
    goto right;

  mv_nPosition = s_min( mv_nPosition, (int) ( nCurColItems - 1 ) );

  p = m_Items( mv_nColumn )->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( true );
  else
    mf_Up();
}

void Menu::mf_SetPosition( const unsigned ac_nPosition, const unsigned ac_nColumn )
{
  const unsigned nCols = m_Items.mf_nGetNumItems();
  if( ac_nColumn >= nCols )
  {
    DBG( "Menu::mf_SetPosition out of range" )
    return;
  }

  const unsigned nItems = m_Items( ac_nColumn )->mf_nGetNumItems();
  if( ac_nPosition >= nItems )
  {
    DBG( "Menu::mf_SetPosition out of range" )
    return;
  }

    //unhighlight the current one
  Component* p = m_Items( mv_nColumn )->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( false );

  mv_nColumn = ac_nColumn;
  mv_nPosition = ac_nPosition;

  p = m_Items( mv_nColumn )->operator()( mv_nPosition );
  if( p )
    p->mp_HighLight( true );
}

void Menu::mf_Select()
{
  mt_ColumnItems* pCur = m_Items( mv_nColumn );
  const unsigned nItems = pCur->mf_nGetNumItems();
  if( nItems == 0 )
    return;

  TRACE( "Selecting" );
  DBGPF( "selecting %d", mv_nPosition );
  pCur->operator()( mv_nPosition )->mp_DoAction();
}
