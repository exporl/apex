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
 
#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "appcore/singleton.h"
#include "appcore/threads/locks.h"
using namespace appcore;

#ifdef USESQT
#if QT_VERSION < 0x040000
#include <qframe.h>
#else
#include <QFrame>
#endif
#include "controller/qt_buttoncontrolemu.h"
using namespace controller;
#endif

namespace uicore
{

  const unsigned PAL  = 12;
  const unsigned NTSC = 10;

    /**
      * FrameBuffer
      *   represents a framebuffer device.
      *   Check google for what this is exactly; basically it just
      *   comes down to the mf_pGetDisplay() function, which gives
      *   access to a buffer where you can put your pixels in.
      *   The buffer is built like this:
      *   @verbatim
          [ row0 ][ row1 ] ... [ row n ]
      *   @endvarbatim
      *   where every row has mf_nGetWidth() pixels.
      *   So if you want to draw a line at row 2, starting at pixel 5,
      *   you fill the memory starting at 2 * mf_nGetWidth() + 5.
      *   The framebuffer must be transparent to the application: no matter
      *   what kind of buffering is used, it must behave like
      *   a single buffer so that the implementations can go wild with pixels
      *   without having to care if they are actually writing to buffer 0 or
      *   1 or n, or directly to a single display buffer.
      *   All methods must be implemented in the platform-specific code,
      *   or in a software emulation.
      *   All methods must *only* be called after a successfull call
      *   to mf_nInitialize(), else trouble is guaranteed.
      *   There can be more then one framebuffer (transparency, OSD, etc),
      *   so it's not a singleton, but there should be only one main display.
      *********************************************************************** */
  class FrameBuffer
#ifdef USESQT
    : public QFrame
#endif
  {
  public:
      /**
        * Constructor.
        */
    FrameBuffer();

      /**
        * Destructor.
        */
    virtual ~FrameBuffer();

      /**
        * Initialize the device.
        * @param ac_psName the device name
        * @return the file descriptor, -1 for error
        */
    int mf_nInitialize( const char* ac_psName );

      /**
        * Get the pointer to the buffer.
        * Must be filled before any switches occur, hence
        * normally within the refresh period.
        * @return a pointer
        */
    unsigned short* mf_pGetDisplay();

      /**
        * Used to get scaling for PAL or NTSC
        * @return the scale factor
        */
    unsigned mf_nGetYFactor() const;

      /**
        * Apply scale according to the Y factor.
        * Not a global function since that factor must
        * be initialized first.
        * @param ac_nY value to scale
        * @return scaled value
        */
    INLINE unsigned mf_nScaleY( const unsigned ac_nY ) const
    {
      return ( ac_nY * mf_nGetYFactor() ) / 10;
    }

      /**
        * Get the display width.
        * @return the width in pixels
        */
    unsigned mf_nGetWidth() const;

      /**
        * Get the display height.
        * @return the height in pixels
        */
    unsigned mf_nGetHeight() const;

      /**
        * Get the bit depth for the colors.
        * @return the depth, 16 or so
        */
    unsigned mf_nGetBPP() const;

      /**
        * Get the total size of the memory
        * pointed to by mf_pGetDisplay.
        * @return the size in bytes
        */
    unsigned mf_nGetScreenSize() const;

      /**
        * Let the device know the buffer contents changed.
        * Normally only used with emulators.
        */
    void mp_SetBufferChanged();

    #ifdef USESQT
    void paintEvent( QPaintEvent* );
    void keyPressEvent( QKeyEvent* );
    void mp_InstallControllerEmu( QtButtonControlEmu* a_pEmu );
    #endif

  private:
    void mp_SetDisplay( unsigned short* a_pDisplay );

    int                   m_fdScreen;
    unsigned              m_nYFactor;
    const CriticalSection mc_Lock;

    #if defined S_ARM || defined USESQT
    unsigned short* m_pFrameBuffer;
    #endif
    #if defined USESQT
    bool mv_bDirty;
    QtButtonControlEmu* m_pEmu;
    #if QT_VERSION > 0x040000
    unsigned* m_pConvertBuf;
    #endif
    #endif
  };

}

#endif //#ifndef __FRAMEBUFFER_H__
