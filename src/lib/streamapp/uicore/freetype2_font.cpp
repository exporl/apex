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
 
#include "freetype2_font.h"
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

using namespace uicore;

namespace
{
  const FT_Encoding sc_Encoding = FT_ENCODING_UNICODE;
}

FreeType2Font::FreeType2Font() :
  mv_bInit( false )
{
  mp_InitColor();
}

FreeType2Font::~FreeType2Font()
{
}

#ifdef _MSC_VER
#pragma warning ( disable : 4244 )  //conv form uint to uchar; is necessary
#endif

void FreeType2Font::mp_InitColor( const tRgb& /*ac_Color*/ )
{
    //Initialize the conversion arrays
  for( unsigned i = 0 ; i < 256 ; ++i )
    m_8bit5[ i ] = i * ( 1 << 5 ) / ( 1 << 8 );

  for( unsigned i = 0 ; i < 256 ; ++i )
    m_8bit6[ i ] = i * ( 1 << 6 ) / ( 1 << 8 );
}

bool FreeType2Font::mp_bInitialize( const char* ac_sFontName )
{
  if( mv_bInit )
    mp_UnInitialize();

  FT_Error error = FT_Init_FreeType( &m_Lib );

  if( error )
  {
    DBG( "Failed to intialize freetype library" );
    return false;
  }

  error = FT_New_Face( m_Lib, ac_sFontName, 0, &m_Face );

  if( error )
  {
    DBGPF( "Failed to load font %s with error %x", ac_sFontName, error );
    return false;
  }

  error = FT_Select_Charmap( m_Face, sc_Encoding );
  if( error )
  {
    DBGPF( "Invalid charmap [%d]", sc_Encoding );
    return false;
  }

  return true;
}

void FreeType2Font::mp_UnInitialize()
{
  FT_Done_Face( m_Face );
  FT_Done_FreeType( m_Lib );
  mv_bInit = false;
}

bool FreeType2Font::mf_bTextSize( const char* ac_psText, const unsigned ac_nPixelHeight,
                                  unsigned& a_nTextWidth, unsigned& ac_nTextHeight )
{
  FT_Set_Char_Size( m_Face, ac_nPixelHeight << 6, ac_nPixelHeight << 6, 100, 100 );

  FT_Vector pen;
  pen.x = 0;
  pen.y = 0;

  int width = 0;
  int height = 0;

  while( *ac_psText )
  {
    FT_Set_Transform( m_Face, NULL, &pen );

    int index = FT_Get_Char_Index( m_Face, *ac_psText);

    FT_Error error = FT_Load_Glyph( m_Face, index, 0 );
    if( error )
    {
      DBGPF( "Failed to load glyph for character %c", *ac_psText );
      break;
    }

    if( m_Face->glyph->format != FT_GLYPH_FORMAT_BITMAP )
    {
      error = FT_Render_Glyph( m_Face->glyph, FT_RENDER_MODE_NORMAL );
      if( error)
      {
        DBGPF( "Failed to render glyph for character %c", *ac_psText );
        break;
      }
    }

    width += m_Face->glyph->bitmap.width + 1;
    if( m_Face->glyph->bitmap.rows > height )
      height = m_Face->glyph->bitmap.rows;

    pen.x += m_Face->glyph->advance.x;
    pen.y += m_Face->glyph->advance.y;
    ac_psText++;
  }

  a_nTextWidth = width;
  ac_nTextHeight = height;

  return true;
}

bool FreeType2Font::mf_bRenderText( const unsigned ac_nX, const unsigned ac_nY, unsigned short* a_pDest, const char* ac_psText )
{
  FT_Set_Char_Size( m_Face, mv_nSize << 6, mv_nSize << 6, 100, 100);

  FT_Vector pen;
  pen.x = ac_nX << 6;
  pen.y = ( mv_nScrH - ac_nY ) << 6;

  //DBGPF("Rendering ac_psText %s", ac_psText);

  while( *ac_psText )
  {
    FT_Set_Transform( m_Face, NULL, &pen );

    int index = FT_Get_Char_Index( m_Face, *ac_psText );

    FT_Error error = FT_Load_Glyph( m_Face, index, 0 );
    if( error )
    {
      DBGPF( "Failed to load glyph for character %c", *ac_psText );
      break;
    }

    if( m_Face->glyph->format != FT_GLYPH_FORMAT_BITMAP )
    {
      error = FT_Render_Glyph( m_Face->glyph, FT_RENDER_MODE_NORMAL );
      if( error)
      {
        DBGPF( "Failed to render glyph for character %c", *ac_psText );
        break;
      }
    }

    int width = m_Face->glyph->bitmap.width;
    int height = m_Face->glyph->bitmap.rows;

    int x = m_Face->glyph->bitmap_left;
    int y = mv_nScrH - m_Face->glyph->bitmap_top;

    unsigned x_max = x + width;
    unsigned y_max = y + height;

    u8 *src = m_Face->glyph->bitmap.buffer;

    for( unsigned i = x, p = 0; i < x_max; ++i, ++p )
    {
      for( unsigned j = y, q = 0; j < y_max; ++j, ++q )
      {
        if( i >= mv_nScrW || j >= mv_nScrH )
          continue;

        int sp = q * width + p;
        int dp = j * mv_nScrW + i;

        u8 pixel = src[ sp ] * mv_nFade / 255;

        a_pDest[ dp ] |= m_8bit5[ pixel ] | m_8bit6[ pixel ] << 5 | m_8bit5[ pixel ] << 11;
      }
    }
    ac_psText++;

    pen.x += m_Face->glyph->advance.x;
    pen.y += m_Face->glyph->advance.y;
  }
  return true;
}
