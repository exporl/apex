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
 
#include "sinegenerator.h"

using namespace streamapp;

SineGenerator::SineGenerator( const unsigned       ac_nChan,
                              const unsigned       ac_nBufferSize,
                              const unsigned long  ac_lSampleRate ) :
  mc_nChan( ac_nChan ),
  mc_nSize( ac_nBufferSize ),
  mc_dSampleRate( (double) ac_lSampleRate  ),
  mv_dSignalAmp( 1.0 ),
  m_dFrequency( 1.0 ),
  m_dPhase( 0.0 ),
  m_dTime( 0.0 ),
  m_Buf( ac_nChan , ac_nBufferSize , true )
{}

SineGenerator::~SineGenerator(  )
{}

const Stream& SineGenerator::Read() 
{
  for( unsigned j = 0 ; j < mc_nSize ; ++j )
  {
    ++m_dTime;
    const double t = m_dTime / mc_dSampleRate;
    const double d = mv_dSignalAmp * sin( m_dFrequency * t + m_dPhase );
    for( unsigned i = 0 ; i < mc_nChan ; ++i )
      m_Buf.mp_Set( i, j, d );
  }

  return m_Buf;
}
