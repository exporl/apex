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
 
#include "message.h"

using namespace appcore;

Message::Message() :
  m_nParam1( 0 ),
  m_nParam2( 0 ),
  m_pParam( 0 ),
  m_pTarget( 0 )    
{
}

Message::Message( const int ac_nParam1, const int ac_nParam2, void* a_pParamP ) :
  m_nParam1( ac_nParam1 ),
  m_nParam2( ac_nParam2 ),
  m_pParam( a_pParamP ),
  m_pTarget( 0 )    
{  
}

Message::~Message()
{
}
