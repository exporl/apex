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
 
#include "component.h"
#include "painter.h"
#include "screen.h"
#include "utils/checks.h"

using namespace uicore;

Component::Component( Screen* a_pParent ) :
  mv_Rect(),
  mv_bDirty( true ),
  mv_bShow( false ),
  mv_bHighLight( false ),
  m_pParent( utils::PtrCheck( a_pParent ) )
{
  m_pParent->mp_AddComponent( this );
}

Component::~Component()
{
  m_pParent->mp_RemoveComponent( this );
}

void Component::mp_ReparentComponent( Screen* a_pNewP )
{
  Screen* newP = utils::PtrCheck( a_pNewP );
  m_pParent->mp_RemoveCompFromList( this );
  newP->mp_AddComponent( this );
  m_pParent = newP;
  mp_SetDirty();
}

void Component::mf_Draw()
{
  if( !mv_bDirty )
    return;

  if( mv_bShow )
    mf_Render();
}

void Component::mf_ReDraw()
{
  if( mv_bShow )
    mf_Render();
}

void Component::mp_SetSize( const unsigned ac_nX, const unsigned ac_nY )
{
  mp_SetDirty();
  mv_Rect.w = s_min( ac_nX, m_pParent->mf_nWidth() - mv_Rect.x );
  mv_Rect.h = s_min( ac_nY, m_pParent->mf_nHeight() - mv_Rect.y );
}

void Component::mp_SetPosition( const unsigned ac_nX, const unsigned ac_nY )
{
  mp_SetDirty();
  mv_Rect.x = s_min( ac_nX, m_pParent->mf_nWidth() - mv_Rect.w );
  mv_Rect.y = s_min( ac_nY, m_pParent->mf_nHeight() - mv_Rect.h );
}

void Component::mp_SetGeometry( const tRect& ac_Rect )
{
  mp_SetDirty();
  mv_Rect.x = s_min( ac_Rect.x, m_pParent->mf_nWidth() - ac_Rect.w );
  mv_Rect.y = s_min( ac_Rect.y, m_pParent->mf_nHeight() - ac_Rect.h );
  mv_Rect.w = s_min( ac_Rect.w, m_pParent->mf_nWidth() - ac_Rect.x );
  mv_Rect.h = s_min( ac_Rect.h, m_pParent->mf_nHeight() - ac_Rect.y );
}
