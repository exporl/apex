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
 
#include "text.h"
#include "screen.h"
#include "fontfactory.h"
#include "painter.h"
#include <assert.h>
#include <cstring>
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

using namespace uicore;

namespace
{
  const u8 PRESSED_OFFSET =  0x30;    //Color difference for pressed button.
}

TextComponent::TextComponent( Screen* a_pParent, const char* ac_psText ) :
    Component( a_pParent ),
  m_pFont( FontFactory::sf_pCreateFont() ),
  mv_bEraseFirst( false )
{
  if( !m_pFont )
    assert( 0 );
  m_pFont->mp_SetScreenHeight( a_pParent->mf_nHeight() );
  m_pFont->mp_SetScreenWidth( a_pParent->mf_nWidth() );
  for( unsigned i = 0 ; i < sc_nMaxTextLen ; ++i )
    mv_psText[ i ] = 0;
  mp_SetText( ac_psText );
}

TextComponent::~TextComponent()
{
  delete m_pFont;
}

void TextComponent::mf_RenderText( Painter* a_pPainter )
{
  static unsigned nStringW = 0;
  static unsigned nStringH = 0;

  if( !m_pFont->mf_bTextSize( mv_psText, m_pFont->mf_nGetFontSize(), nStringW, nStringH ) )
    return;

  //DBGPF( "Received string text length = %d and height = %d\n", nStringW, nStringH );

  if( nStringW > mv_Rect.w || nStringH > mv_Rect.h )
  {
    DBG( "String larger than button size!" );
    return;
  }

  unsigned x = mv_Rect.x + mv_Rect.w / 2 - nStringW / 2;
  unsigned y = mv_Rect.y + mv_Rect.h / 2 + nStringH / 2;

  a_pPainter->mf_DrawText( x, y, mv_psText, *m_pFont );

  //DBG( "font rendered" )
}

void TextComponent::mf_Render()
{
  static Painter painter( m_pParent );

  if( mv_Rect.h == 0 || mv_Rect.w == 0 || strlen( mv_psText ) == 0 )
    return;
  if( mv_bDirty )
  {
    if( mv_bEraseFirst )
      painter.mf_Clear( mv_Rect, m_pParent->mf_tGetEraseColor() );
    mf_RenderText( &painter );
  }
}

void TextComponent::mp_SetText( const char* ac_psText )
{
  mp_SetDirty();
  if( ac_psText )
  {
    const size_t nToCopy = s_min( strlen( ac_psText ), sc_nMaxTextLen );
    strncpy( mv_psText, ac_psText, nToCopy );
  }
}

void TextComponent::mp_SetFont( const Font& a_Font )
{
  *m_pFont = a_Font;
}

Font& TextComponent::mp_GetFont()
{
  return *m_pFont;
}


