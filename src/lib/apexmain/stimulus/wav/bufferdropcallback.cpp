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
 
#include "bufferdropcallback.h"
#include "apexcontrol.h"
#include "error/errorevent.h"
#include <QApplication>

using namespace apex;
using namespace apex::stimulus;

BufferDropCallback::BufferDropCallback( const QString& ac_sDropSource ) :
  mv_nDrops( 0 ),
  m_pError( StatusItem::ERROR, ac_sDropSource, "!!!!! buffer underrun !!!!!" ) 
{
}

BufferDropCallback::~BufferDropCallback()
{
}

void BufferDropCallback::mf_Callback()
{
    //ok it's not nice to start allocating in the audio thread,
    //but buffers already dropped so we don't bother with it
  QApplication::postEvent( &ApexControl::Get(), new ErrorEvent( m_pError ) );
  ++mv_nDrops;
}
