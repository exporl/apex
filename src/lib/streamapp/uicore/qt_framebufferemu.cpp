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
 
  /**
    * @file qt_framebufferemu.cpp
    * A FrameBuffer emulation using Qt. Slow as hell, even with Qt4 where QImage is used,
    * and it eats cpu.
    */

#include "uicore/framebuffer.h"
#include "uicore/gfx.h"
#include <qimage.h>
#include <qpainter.h>
#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

#if QT_VERSION > 0x040000
#include <QKeyEvent>
#endif

namespace
{
  const unsigned nBPP = 16;
  const unsigned nWidth = 720;
  const unsigned nHeight = 480;
}

uicore::FrameBuffer::FrameBuffer() :
  #if QT_VERSION < 0x040000
    QFrame( 0, "fbemu", 0 ),
  #else
    QFrame( 0 ),
  #endif
  m_fdScreen( 0 ),
  m_nYFactor( PAL ),
  m_pFrameBuffer( new unsigned short[ nWidth * nHeight ] ),
#if QT_VERSION > 0x040000
  m_pConvertBuf( new unsigned[ nWidth * nHeight ] ),
#endif
  mv_bDirty( false ), //don't draw until something is rendered on the screen
  m_pEmu( 0 )
{
  QFrame::setGeometry( QRect( 50, 50, nWidth, nHeight ) );
  QFrame::show();
}

uicore::FrameBuffer::~FrameBuffer()
{
  const Lock L( mc_Lock );
  delete [] m_pFrameBuffer;
#if QT_VERSION > 0x040000
  delete [] m_pConvertBuf;
#endif
}

void uicore::FrameBuffer::paintEvent( QPaintEvent* /*e*/ )
{
  if( mv_bDirty )
  {
    const Lock L( mc_Lock );
    QPainter painterken( this );

#if QT_VERSION > 0x040000
      //convert color to QRGB (quite lossy!) and
      //paint a QImage with it.
    for( unsigned i = 0 ; i < nHeight ; ++i )
    {
      for( unsigned j = 0 ; j < nWidth ; ++j )
        m_pConvertBuf[ nWidth * i + j ] = m_pFrameBuffer[ nWidth * i + j ] | 0xFF000000;
    }
    QImage image( (uchar*) m_pConvertBuf, nWidth, nHeight, QImage::Format_RGB32 );
    painterken.drawImage( 0, 0, image );
#else
      //just copy the entire buffer to the screen,pixel by pixel.
      //terribly slow, but I see no other way to do this,
      //it doesn't seem possible to copy an entire buffer to qt.
    QPen pen;
    pen.setWidth( 1 );
    for( unsigned i = 0 ; i < nHeight ; ++i )
    {
      for( unsigned j = 0 ; j < nWidth ; ++j )
      {
        tRgb col( tRgb::sf_ColorConvert( m_pFrameBuffer[ nWidth * i + j ] ) );
        pen.setColor( QColor( col.r, col.g, col.b ) );
        painterken.setPen( pen );
        painterken.drawPoint( j, i );
      }
    }
#endif
    mv_bDirty = false;
  }
}

void uicore::FrameBuffer::mp_InstallControllerEmu( QtButtonControlEmu* a_pEmu )
{
  m_pEmu = a_pEmu;
}

void uicore::FrameBuffer::keyPressEvent( QKeyEvent* e )
{
  if( m_pEmu )
    m_pEmu->mf_KeyPressed( e->key() );
}

int uicore::FrameBuffer::mf_nInitialize( const char* /*ac_psName*/ )
{
  return m_fdScreen;
}

unsigned short* uicore::FrameBuffer::mf_pGetDisplay()
{
  const Lock L( mc_Lock );
  return m_pFrameBuffer;
}

unsigned uicore::FrameBuffer::mf_nGetYFactor() const
{
  const Lock L( mc_Lock );
  return m_nYFactor;
}

unsigned uicore::FrameBuffer::mf_nGetWidth() const
{
  return nWidth;
}

unsigned uicore::FrameBuffer::mf_nGetHeight() const
{
  return nHeight;
}

unsigned uicore::FrameBuffer::mf_nGetBPP() const
{
  return nBPP;
}

unsigned uicore::FrameBuffer::mf_nGetScreenSize() const
{
  return mf_nGetWidth() * mf_nGetHeight() * mf_nGetBPP() / 8;
}

void uicore::FrameBuffer::mp_SetBufferChanged()
{
  const Lock L( mc_Lock );
  mv_bDirty = true;
#if QT_VERSION < 0x040000
  QFrame::repaint( false );
#else
  QFrame::repaint();
#endif
}
