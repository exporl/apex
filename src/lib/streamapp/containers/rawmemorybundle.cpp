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
 
#include "rawmemorybundle.h"
#include "utils/checks.h"

 #include <QtGlobal>        // Q_ASSERT

using namespace utils;
using namespace streamapp;

RawMemoryBundler::RawMemoryBundler() :
  mv_nPos( 0 ),
  m_pMem( 0 )
{
}

RawMemoryBundler::RawMemoryBundler( RawMemory* const ac_pTargetBundle ) :
  mv_nPos( 0 ),
  m_pMem( ac_pTargetBundle )
{
}

RawMemoryBundler::~RawMemoryBundler()
{
}

void RawMemoryBundler::mp_SetBundle( RawMemory* const ac_pToBundle )
{
  m_pMem = PtrCheck( ac_pToBundle );
  mv_nPos = 0;
}

void RawMemoryBundler::mp_SetWritePosition( const unsigned ac_nByteOffset )
{
  Q_ASSERT( m_pMem );
  mv_nPos = ac_nByteOffset;
}

RawMemoryAccess RawMemoryBundler::mf_GetBundledData() const
{
  Q_ASSERT( m_pMem );
  return m_pMem->mf_GetAccess( 0, mv_nPos );
}

RawMemoryUnBundler::RawMemoryUnBundler() :
  mv_nPos( 0 ),
  mc_pMem( 0 )
{
}

RawMemoryUnBundler::RawMemoryUnBundler( RawMemoryAccess* const ac_pTargetBundle ) :
  mv_nPos( 0 ),
  mc_pMem( ac_pTargetBundle )
{
}

RawMemoryUnBundler::~RawMemoryUnBundler()
{
}

void RawMemoryUnBundler::mp_SetBundle( const RawMemoryAccess* const ac_pBundle )
{
  mc_pMem = PtrCheck( ac_pBundle );
  mv_nPos = 0;
}

void RawMemoryUnBundler::mp_SetReadPosition( const unsigned ac_nByteOffset )
{
  Q_ASSERT( mc_pMem && ac_nByteOffset < mc_pMem->mf_nGetSize() );
  mv_nPos = ac_nByteOffset;
}
