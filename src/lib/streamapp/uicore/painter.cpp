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
 
#include "painter.h"
#include "screen.h"
#include "font.h"
#include "gfx.h"

using namespace uicore;

Painter::Painter( Screen* const a_pScreen ) :
  mc_pScreen( a_pScreen )
{
}

  Painter::Painter( const Painter& ac_Rh ) :
  mc_pScreen( ac_Rh.mc_pScreen )
{
}

Painter::~Painter()
{
}

void Painter::mf_Clear( const tRgb& ac_Color /*= tRgb::sc_Black()*/ )
{
  mf_Clear( 0, 0, mc_pScreen->mf_nWidth(), mc_pScreen->mf_nHeight(), ac_Color );  //FIXME just do a big memset
}

void Painter::mf_Clear( const tRect& ac_Area, const tRgb& ac_Color )
{
  mf_Clear( ac_Area.x, ac_Area.y, ac_Area.w, ac_Area.h, ac_Color );
}

void Painter::mf_Clear( const unsigned ac_nX, const unsigned ac_nY, const unsigned ac_nW, const unsigned ac_nH, const tRgb& ac_Color )
{
  const unsigned& c_nW = mc_pScreen->mf_nWidth();
  const unsigned nW = s_min( ac_nW, c_nW );
  const unsigned nH = s_min( ac_nH, mc_pScreen->mf_nHeight() );

  unsigned short* ptr = mc_pScreen->mf_pGetDisplay() + ac_nY * c_nW + ac_nX;
  const unsigned nToClear = nW * 2;
  for( unsigned i = 0 ; i < nH ; ++i )
  {
    memset( ptr, ac_Color.mf_ColorConvert(), nToClear );
    ptr += c_nW;
  }
}

void Painter::mf_FillRectangle( const unsigned x, const unsigned y, const unsigned w, const unsigned h, unsigned short col )
{
  const unsigned& nW = mc_pScreen->mf_nWidth();
  unsigned short* dst = mc_pScreen->mf_pGetDisplay() + y * nW + x;

  for( unsigned j = 0 ; j < h ; ++j )
  {
    for( unsigned i = 0 ; i < w ; ++i )
      dst[ i ] = col;
    dst += nW;
  }
}

void Painter::mf_FillRectangle( const tRect& ac_Rectangle, unsigned short col )
{
  const unsigned& nW = mc_pScreen->mf_nWidth();
  unsigned short* dst = mc_pScreen->mf_pGetDisplay() + ac_Rectangle.y * nW + ac_Rectangle.x;
  
  for( unsigned j = 0 ; j < ac_Rectangle.h ; ++j )
  {
    for( unsigned i = 0 ; i < ac_Rectangle.w ; ++i )
      dst[ i ] = col;
    dst += nW;
  }
}

void Painter::mf_DrawRectangle( unsigned x, unsigned y, unsigned w, unsigned h, const u8 r, const u8 g, const u8 b )
{
  mf_FillRectangle( x, y, w, h, tRgb::sf_ColorConvert( r, g, b ) );
}

namespace
{
    //Maximum height of screen supported is 1024 pixels
  unsigned int limits[ 1024 ][ 2 ];

  const unsigned MIN = 0;
  const unsigned MAX = 1;

  void gf_LineCompare( int x0, int y0, int x1, int y1 )
  {
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;

    if( dy < 0 )
    {
      dy = -dy;
      stepy = -1;
    }
    else
    {
      stepy = 1;
    }
    if( dx < 0 )
    {
      dx = -dx;
      stepx = -1;
    }
    else
    {
      stepx = 1;
    }
    dy <<= 1;
    dx <<= 1;

    if( (unsigned) x0 < limits[ y0 ][ MIN ] )
      limits[ y0 ][ MIN ] = x0;
    if( (unsigned) x0 > limits[ y0 ][ MAX ] )
      limits[ y0 ][ MAX ] = x0;

    if( dx > dy )
    {
      int fraction = dy - ( dx >> 1 );

      while( x0 != x1 )
      {
        if( fraction >= 0 )
        {
          y0 += stepy;
          fraction -= dx;
        }
        x0 += stepx;
        fraction += dy;
        if( (unsigned) x0 < limits[ y0 ][ MIN ] )
          limits[ y0 ][ MIN ] = x0;
        if( (unsigned) x0 > limits[ y0 ][ MAX ] )
          limits[ y0 ][ MAX ] = x0;
      }
    }
    else
    {
      int fraction = dx - ( dy >> 1 );

      while( y0 != y1 )
      {
        if( fraction >= 0 )
        {
          x0 += stepx;
          fraction -= dy;
        }
        y0 += stepy;
        fraction += dx;
        if( (unsigned) x0 < limits[ y0 ][ MIN ] )
          limits[ y0 ][ MIN ] = x0;
        if( (unsigned) x0 > limits[ y0 ][ MAX ])
          limits[ y0 ][ MAX ] = x0;
      }
    }
  }

  void gf_LineCompareInit( unsigned miny, unsigned maxy )
  {
    for( unsigned y = miny ; y <= maxy ; ++y )
    {
      limits[ y ][ MIN ] = 0xffffffff;
      limits[ y ][ MAX ] = 0;
    }
  }
}

void Painter::mf_FillTriangle( unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned x3, unsigned y3, unsigned short col )
{
  unsigned miny = y1;
  if( y2 < miny )
    miny = y2;
  if( y3 < miny )
    miny = y3;

  unsigned maxy = y1;
  if( y2 > maxy )
    maxy = y2;
  if( y3 > maxy )
    maxy = y3;

    //fill limits with extrema values
  gf_LineCompareInit( miny, maxy );

    //find max and min X values for all used Y values
  gf_LineCompare( x1, y1, x2, y2 );
  gf_LineCompare( x2, y2, x3, y3 );
  gf_LineCompare( x3, y3, x1, y1 );

  const unsigned& nW = mc_pScreen->mf_nWidth();
  unsigned short* pos = mc_pScreen->mf_pGetDisplay() + miny * nW;

    //draw horizontally between max and min X and max and min Y
  for( unsigned y = miny ; y <= maxy ; ++y )
  {
    for( unsigned x = limits[ y ][ MIN ] ; x <= limits[ y ][ MAX ] ; ++x )
      pos[ x ] = col;
    pos += nW;
  }
}

void Painter::mf_DrawTriangle( unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned x3, unsigned y3, const u8 r, const u8 g, const u8 b )
{
  mf_FillTriangle( x1, y1, x2, y2, x3, y3, tRgb::sf_ColorConvert( r, g, b ) );
}

void Painter::mf_DrawText( const unsigned ac_nX, const unsigned ac_nY, const char* ac_pText, Font& a_Font )
{
  a_Font.mf_bRenderText( ac_nX, ac_nY, mc_pScreen->mf_pGetDisplay(), ac_pText );
}
