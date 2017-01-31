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
 
#include "uicore/framebuffer.h"

#ifdef S_DEBUG
#include "utils/tracer.h"
#endif

//uicore::FrameBuffer uicore::MainFrameBuffer;

void uicore::FrameBuffer::mp_SetDisplay( unsigned short* a_pDisplay )
{
  DBG( "framebuffer set" )
  m_pFrameBuffer = a_pDisplay;
}

 unsigned short* uicore::FrameBuffer::mf_pGetDisplay()
{
  return m_pFrameBuffer;
}

unsigned uicore::FrameBuffer::mf_nGetWidth() const
{
  return 720;
}

unsigned uicore::FrameBuffer::mf_nGetHeight() const
{
  return mf_nScaleY( 480 );
}

unsigned uicore::FrameBuffer::mf_nGetBPP() const
{
  return 16;
}

unsigned uicore::FrameBuffer::mf_nGetScreenSize() const
{
  return mf_nGetWidth() * mf_nGetHeight() * mf_nGetBPP() / 8;
}
