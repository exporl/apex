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
 
#include "button.h"
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
  const u8 DEPTH_OFFSET   =  0x10;    //Color difference for button borders.
  const u8 PRESSED_OFFSET =  0x30;    //Color difference for pressed button.
}

Button::Button( Screen* a_pParent ) :
    TextComponent( a_pParent ),
  mv_BaseColor(),
  mv_bFlat( false ),
  mv_nBorder( 0 )
{
}

Button::~Button()
{
}

void Button::mf_RenderButton( Painter* a_pPainter, const u8 ac_nR, const u8 ac_nG, const u8 ac_nB )
{
    //draw the background of the button
  a_pPainter->mf_FillRectangle( mv_Rect, tRgb::sf_ColorConvert( ac_nR, ac_nG, ac_nB ) );

  if( !mv_bFlat )
  {
    u8 dr = ( ac_nR - DEPTH_OFFSET < 0 )   ? 0   : ac_nR - DEPTH_OFFSET;
    u8 dg = ( ac_nG - DEPTH_OFFSET < 0 )   ? 0   : ac_nG - DEPTH_OFFSET;
    u8 db = ( ac_nB - DEPTH_OFFSET < 0 )   ? 0   : ac_nB - DEPTH_OFFSET;
    u8 lr = ( ac_nR + DEPTH_OFFSET > 255 ) ? 255 : ac_nR + DEPTH_OFFSET;
    u8 lg = ( ac_nG + DEPTH_OFFSET > 255 ) ? 255 : ac_nG + DEPTH_OFFSET;
    u8 lb = ( ac_nB + DEPTH_OFFSET > 255 ) ? 255 : ac_nB + DEPTH_OFFSET;

    unsigned short dark_col = tRgb::sf_ColorConvert( dr, dg, db);
    unsigned short light_col = tRgb::sf_ColorConvert( lr, lg, lb );

      //draw the button depth
    a_pPainter->mf_FillRectangle( mv_Rect.x, mv_Rect.y + mv_Rect.h - mv_nBorder, mv_Rect.w, mv_nBorder, dark_col);
    a_pPainter->mf_FillRectangle( mv_Rect.x, mv_Rect.y, mv_nBorder, mv_Rect.h, dark_col);
    a_pPainter->mf_FillRectangle( mv_Rect.x+mv_nBorder, mv_Rect.y, mv_Rect.w-mv_nBorder, mv_nBorder, light_col);
    a_pPainter->mf_FillTriangle ( mv_Rect.x, mv_Rect.y, mv_Rect.x+mv_nBorder, mv_Rect.y+mv_nBorder, mv_Rect.x+(mv_nBorder*2), mv_Rect.y, light_col);
    a_pPainter->mf_FillRectangle( mv_Rect.x+mv_Rect.w-mv_nBorder, mv_Rect.y, mv_nBorder, mv_Rect.h-mv_nBorder, light_col);
    a_pPainter->mf_FillTriangle ( mv_Rect.x+mv_Rect.w-1, mv_Rect.y+mv_Rect.h-1, mv_Rect.x+mv_Rect.w-1-mv_nBorder, mv_Rect.y+mv_Rect.h-1-mv_nBorder, mv_Rect.x+mv_Rect.w-1, mv_Rect.y+mv_Rect.h-1-mv_nBorder, light_col);
  }
}

void Button::mf_Render()
{
  static Painter painter( m_pParent );

  if( mv_Rect.h == 0 || mv_Rect.w == 0 )
    return;

    //button
  if( !mv_bHighLight )
    mf_RenderButton( &painter, mv_BaseColor.r, mv_BaseColor.g, mv_BaseColor.b );
  else
    mf_RenderButton( &painter, mv_BaseColor.r + PRESSED_OFFSET, mv_BaseColor.g + PRESSED_OFFSET, mv_BaseColor.b + PRESSED_OFFSET );

    //text
  mf_RenderText( &painter );
}

void Button::mp_SetBaseColor( const u8 ac_nR, const u8 ac_nG, const u8 ac_nB )
{
  mp_SetDirty();
  mv_BaseColor.mp_Set( ac_nR, ac_nG, ac_nB );
}

void Button::mp_SetBaseColor( const tRgb& ac_Color )
{
  mp_SetDirty();
  mv_BaseColor = ac_Color;
}

void Button::mp_SetBorder( const unsigned ac_nW )
{
  mp_SetDirty();
  mv_nBorder = ac_nW;
}

void Button::mp_SetFlat( const bool ac_bSet )
{
  mp_SetDirty();
  mv_bFlat = ac_bSet;
}
