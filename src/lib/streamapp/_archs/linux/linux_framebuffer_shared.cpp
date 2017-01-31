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
  * @file
  * common linux implementation of FrameBuffer.
  * The getter methods should be defined elsewhere.
  */ 

#include "uicore/framebuffer.h"

#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>

#define FBIO_GETSTD _IOR( 'F', 0x25, u_int32_t )

uicore::FrameBuffer::FrameBuffer() :
  m_fdScreen( -1 ),
  m_nYFactor( 0 )
  #ifdef S_ARM
  , m_pFrameBuffer( 0 )
  #endif
{
}
    
uicore::FrameBuffer::~FrameBuffer()
{
  const Lock L( mc_Lock );
  if( m_fdScreen != -1 )
    close( m_fdScreen );      
}

int uicore::FrameBuffer::mf_nInitialize( const char* ac_psDevName )
{
  const Lock L( mc_Lock );

  if( m_fdScreen != -1 )
    close( m_fdScreen );

  m_fdScreen = open( ac_psDevName, O_RDWR ); //FIXMEEE do we need to close if open failed??

  if( m_fdScreen == -1 )
  {
    DBGPF( "Failed to open fb device %s", ac_psDevName );
    return m_fdScreen;
  }

  fb_var_screeninfo varInfo;
  if( ioctl( m_fdScreen, FBIOGET_VSCREENINFO, &varInfo ) == -1 )
  {
    DBGPF( "Failed ioctl FBIOGET_VSCREENINFO on %s", ac_psDevName );
    return m_fdScreen = -1;
  }

  int std = 0;
  if( ioctl( m_fdScreen, FBIO_GETSTD, &std ) == -1 )
  {
    DBG( "Failed to get video standard from display device driver" );
    return m_fdScreen = -1;
  }

  if( ( std >> 16 ) == 0x1 )
  {
    DBG( "setting NTSC mode" )
    m_nYFactor = NTSC;
  }
  else
  {
    DBG( "setting PAL mode" )    
    m_nYFactor = PAL;
  }

    //try requested size
  varInfo.xres            = mf_nGetWidth();
  varInfo.yres            = mf_nGetHeight();
  varInfo.bits_per_pixel  = mf_nGetBPP();

  if( ioctl( m_fdScreen, FBIOPUT_VSCREENINFO, &varInfo ) == -1 )
  {
    DBGPF( "Failed ioctl FBIOPUT_VSCREENINFO on %s", ac_psDevName );
    return m_fdScreen = -1;
  }

  if( varInfo.xres != mf_nGetWidth()  ||
      varInfo.yres != mf_nGetHeight() ||
      varInfo.bits_per_pixel != mf_nGetBPP() ) 
  {
     DBG( "Failed to get the requested screen size" );
  }
  else
  {
    DBGPF( "got screen: %d wide, %d high, %d colordepth", varInfo.xres, varInfo.yres, varInfo.bits_per_pixel ); 
  }

  unsigned short* display = (unsigned short *) mmap( 0, mf_nGetScreenSize(), 
                                                      PROT_READ | PROT_WRITE, 
                                                      MAP_SHARED, m_fdScreen, 0 );

  if( display == MAP_FAILED )
  {
    DBGPF( "Failed mmap on %s", ac_psDevName );
    return m_fdScreen = -1;
  }

  mp_SetDisplay( display );
  
  DBG( "framebuffer initialized" )

  return m_fdScreen;
}

unsigned uicore::FrameBuffer::mf_nGetYFactor() const
{
  const Lock L( mc_Lock );
  return m_nYFactor;  
}
