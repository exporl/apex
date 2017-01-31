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

#include "multiwavefile.h"
#include "streamappfactory.h"
#include "utils/stringutils.h"
#include "utils/systemutils.h"

using namespace streamapp;

MultiOutputWaveFile::MultiOutputWaveFile()
{}

MultiOutputWaveFile::~MultiOutputWaveFile()
{
  mp_Close();
}

bool MultiOutputWaveFile::mp_bOpen( const unsigned ac_Channels, const unsigned ac_nBufferSize, const unsigned long ac_lSampleRate, const std::string& ac_sFileNamePrefix, const AudioFormat::mt_eBitMode ac_eBitMode )
{
  mp_Close();
  for( unsigned i = 0 ; i < ac_Channels ; ++i )
  {
    OutputStream* p = StreamAppFactory::sf_pInstance()->mf_pWriterStream( ac_sFileNamePrefix + "_ch" + toString( i ) + ".wav", 1, ac_lSampleRate, ac_nBufferSize, ac_eBitMode );
    MultiOutputStream<>::mp_AddItem( p );
  }
  return true;
}

void MultiOutputWaveFile::mp_Close()
{
  MultiOutputStream<>::mp_RemoveAllItems();
}

/**************************************************************************************************************/

#ifdef _MSC_VER
#pragma warning ( disable : 4706 )    //assignment in conditional expression
#endif

MultiInputWaveFile::MultiInputWaveFile()
{}

MultiInputWaveFile::~MultiInputWaveFile()
{
  mp_Close();
}

bool MultiInputWaveFile::mp_bOpen( const unsigned ac_nBufferSize, const std::string& ac_sFileNamePrefix )
{
  mp_Close();

  bool      bFound = true;
  unsigned  nFound = 0;

  while( bFound )
  {
    const std::string sCur( ac_sFileNamePrefix + "_ch" + toString( nFound ) + ".wav" );
    bFound = systemutils::f_bFileExists( sCur );
    if( bFound )
    {
      ++nFound;
      PositionableInputStream* p = StreamAppFactory::sf_pInstance()->mf_pReaderStream( sCur, ac_nBufferSize );
      MultiPosInputStream<>::mp_AddItem( p );
    }
  }

  if( nFound )
    return true;
  return false;
}

void MultiInputWaveFile::mp_Close()
{
  MultiPosInputStream<>::mp_RemoveAllItems();
}
