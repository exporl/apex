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
 
#include "screen.h"
#include "painter.h"
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

using namespace uicore;

Screen::Screen( FrameBuffer& a_FB ) :
  mc_nW( a_FB.mf_nGetWidth() ),
  mc_nH( a_FB.mf_nGetHeight() ),
  mv_bDirty( true ),
  mv_EraseColor(),
  m_pEraser( 0 ),
  m_FrameBuf( a_FB )
{
  m_pEraser = new Painter( this );
  DBGPF( "created screen with width = %d and height = %d", mc_nW, mc_nH );
}

Screen::~Screen()
{
}

void Screen::mf_Draw()
{
  mf_Erase();
  const unsigned& nComps = m_Comps.mf_nGetNumItems();
  //DBGPF( "drawing %d components", nComps );
  for( unsigned i = 0 ; i < nComps ; ++i )
    m_Comps( i )->mf_ReDraw();
#ifdef USESQT
  m_FrameBuf.mp_SetBufferChanged();
#endif
}

void Screen::mf_DrawNoErase()
{
  if( mv_bDirty ) //erase color changed so we must clear after all
    mf_Erase();
  const unsigned& nComps = m_Comps.mf_nGetNumItems();
  //DBGPF( "drawing %d components", nComps );
  for( unsigned i = 0 ; i < nComps ; ++i )
    m_Comps( i )->mf_Draw();
#ifdef USESQT
  m_FrameBuf.mp_SetBufferChanged();
#endif
  mv_bDirty = false;
}

INLINE void Screen::mf_Erase()
{
  m_pEraser->mf_Clear( mv_EraseColor );
}

void Screen::mp_SetEraseColor( const u8 ac_nR, const u8 ac_nG, const u8 ac_nB )
{
  mv_bDirty = true;
  mv_EraseColor.mp_Set( ac_nR, ac_nG, ac_nB );
}

void Screen::mp_AddComponent( Component* a_pComponent )
{
  m_Comps.mp_AddItem( a_pComponent );
}

void Screen::mp_RemoveComponent( Component* a_pComponent )
{
  m_Comps.mp_RemoveItem( a_pComponent );
}

void Screen::mp_RemoveAllComponents()
{
  m_Comps.mp_RemoveAllItems();
}

void Screen::mp_RemoveCompFromList( Component* a_pComponent )
{
  m_Comps.mp_RemoveItemNoDelete( a_pComponent );
}
