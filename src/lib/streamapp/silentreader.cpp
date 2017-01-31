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

#include "silentreader.h"
#include "utils/dataconversion.h"

#include <cstring>

using namespace streamapp;

SilentReader::SilentReader( const unsigned  ac_nChan, const unsigned ac_nFs, const unsigned long ac_nToGenerate ) :
  mc_lSampleRate( ac_nFs ),
  mc_lSamples( ac_nToGenerate ),
  mc_nChannels( ac_nChan ),
  mv_lGenerated( 0l )
{
}

SilentReader::SilentReader( const unsigned  ac_nChan, const unsigned ac_nFs, const double ac_dMsec ) :
  mc_lSampleRate( ac_nFs ),
  mc_lSamples( dataconversion::gf_nSamplesFromMsec< unsigned long >( (double) ac_nFs, ac_dMsec ) ),
  mc_nChannels( ac_nChan ),
  mv_lGenerated( 0l )
{
}

SilentReader::~SilentReader()
{
}

unsigned long SilentReader::Read( void** a_pBuf, const unsigned ac_nSamples )
{
  if( mv_lGenerated >= mc_lSamples )
    return 0L;

  for( unsigned i = 0 ; i < mc_nChannels ; ++i )
    memset( a_pBuf[ i ], 0, ac_nSamples * sizeof( int ) ); //redundant most of the time...
  mv_lGenerated += ac_nSamples;
  if( mv_lGenerated < mc_lSamples )
    return ac_nSamples;
  else
  {
    const unsigned long ret = mc_lSamples - ( mv_lGenerated - ac_nSamples );
    mv_lGenerated = mc_lSamples;
    return ret;
  }
}

unsigned long SilentReader::mf_lSamplesLeft() const
{
  return mf_lTotalSamples() - mf_lCurrentPosition();
}

unsigned long SilentReader::mf_lTotalSamples() const
{
//    qCDebug(APEX_SA, "SilentReader::mf_lTotalSamples(): %u", mc_lSamples);
  return mc_lSamples;
}

unsigned long SilentReader::mf_lCurrentPosition() const
{
  return mv_lGenerated;
}

void SilentReader::mp_SeekPosition( const unsigned long ac_nPosition )
{
  mv_lGenerated = ac_nPosition;
}
