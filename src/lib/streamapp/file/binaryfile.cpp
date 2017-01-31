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
 
#include "binaryfile.h"
#include "utils/stringutils.h"
#include "utils/systemutils.h"
#include "utils/stringexception.h"
#include "appcore/deleter.h"
#include <iostream>

using namespace streamapp;
using namespace appcore;

namespace
{
  const std::string sc_sExtension( ".sbin" );
  const std::string sc_sChannelPr( "_ch" );
}

BinaryOutputFile::BinaryOutputFile() :
  m_pFiles( 0 )
{
}

BinaryOutputFile::~BinaryOutputFile()
{
  mp_bClose();
}

bool BinaryOutputFile::mp_bOpen( const std::string& ac_sFileNamePrefix, const unsigned ac_nChannels )
{
  if( m_pFiles )
    return false;

  m_pFiles = new mt_Files( ac_nChannels );

  for( unsigned i = 0 ; i < ac_nChannels ; ++ i )
  {
    const std::string sFileName = ac_sFileNamePrefix + sc_sChannelPr + toString( i ) + sc_sExtension;
    m_pFiles->mp_Set( i, new std::ofstream( sFileName.data(), std::ios::out | std::ios::binary ) );
  }

  return true;
}

unsigned BinaryOutputFile::mf_nChannels() const
{
  if( m_pFiles )
    return m_pFiles->mf_nGetBufferSize();
  return 0;
}

bool BinaryOutputFile::mp_bClose()
{
  if( m_pFiles )
  {
    const unsigned c_nChan = mf_nChannels();
    for( unsigned i = 0 ; i < c_nChan ; ++i )
    {
      std::ofstream* pCur = m_pFiles->mf_Get( i );
      pCur->close();
      delete pCur;
    }
    ZeroizingDeleter()( m_pFiles );
    return true;
  }
  return false;
}

void BinaryOutputFile::Write( const Stream& ac_Data )
{
  if( !m_pFiles )
    return;

  const unsigned c_nChan = mf_nChannels();
  assert( ac_Data.mf_nGetChannelCount() >= c_nChan );

  const unsigned c_nSize = ac_Data.mf_nGetBufferSize() * sizeof( StreamType );

  for( unsigned i = 0 ; i < c_nChan ; ++ i )
  {
    std::ofstream* pCur = m_pFiles->mf_Get( i );
    pCur->write( (char*) ac_Data.mf_pGetArray()[ i ], c_nSize );
  }
}

unsigned long BinaryOutputFile::Write( const void** a_pBuf, const unsigned ac_nSamples )
{
  if( !m_pFiles )
    return 0L;

  const unsigned c_nChan = mf_nChannels();
  const unsigned c_nSize = ac_nSamples * sizeof( StreamType );

  for( unsigned i = 0 ; i < c_nChan ; ++ i )
  {
    std::ofstream* pCur = m_pFiles->mf_Get( i );
    pCur->write( (char*) ( ( (StreamType**) a_pBuf )[ i ] ), c_nSize );
  }

  return ac_nSamples;
}

/***************************************************************************************************************************/

BinaryInputFile::BinaryInputFile() :
  m_pFiles( 0 ),
  mv_nSamples( 0 ),
  mv_nReadOffset( 0 )
{
}

BinaryInputFile::~BinaryInputFile()
{
  mp_bClose();
}

bool BinaryInputFile::mp_bOpen( const std::string& ac_sFileNamePrefix )
{
  if( m_pFiles )
    return false;

    //search files
  unsigned nChannels = 0;
  std::string sFileName = ac_sFileNamePrefix + sc_sChannelPr + toString( nChannels ) + sc_sExtension;
  while( systemutils::f_bFileExists( sFileName ) )
  {
    ++nChannels;
    sFileName = ac_sFileNamePrefix + sc_sChannelPr + toString( nChannels ) + sc_sExtension;
  }

  if( !nChannels )
    return false;

  m_pFiles = new mt_Files( nChannels );

  for( unsigned i = 0 ; i < nChannels ; ++ i )
  {
    const std::string sFileName = ac_sFileNamePrefix + sc_sChannelPr + toString( i ) + sc_sExtension;
    std::ifstream* p = new std::ifstream( sFileName.data(), std::ios::in | std::ios::binary );

      //get size
    std::ifstream::pos_type begin = p->tellg();
    p->seekg( 0, std::ios::end );
    std::ifstream::pos_type end = p->tellg();
    p->seekg( 0 );

    if( i == 0 )
    {
      mv_nSamples = ( end - begin ) / sc_nSampleSize;
    }
    else
    {
        //check same size
      if( ( (unsigned long) ( ( end - begin ) / sc_nSampleSize ) ) != mv_nSamples )
      {
          //mismatch; clean up and fail
        for( unsigned j = 0 ; j < i - 1 ; ++j )
        {
          std::ifstream* pCur = m_pFiles->mf_Get( i );
          pCur->close();
          delete pCur;
        }
        ZeroizingDeleter()( m_pFiles );
        return false;
      }
    }

    m_pFiles->mp_Set( i, p );
  }

  mv_nReadOffset = 0;

  return true;
}

bool BinaryInputFile::mp_bOpenAny( const std::string& ac_sFileName )
{
  if( m_pFiles )
    return false;

  if( !systemutils::f_bFileExists( ac_sFileName ) )
    return false;

  std::ifstream* p = new std::ifstream( ac_sFileName.data(), std::ios::in | std::ios::binary );
  m_pFiles = new mt_Files( 1 );
  m_pFiles->mp_Set( 0, p );

    //get size
  std::ifstream::pos_type begin = p->tellg();
  p->seekg( 0, std::ios::end );
  std::ifstream::pos_type end = p->tellg();
  p->seekg( 0 );

  mv_nSamples = ( end - begin ) / sc_nSampleSize;
  mv_nReadOffset = 0;

  return true;
}

bool BinaryInputFile::mp_bClose()
{
  if( m_pFiles )
  {
    const unsigned c_nChan = mf_nChannels();
    for( unsigned i = 0 ; i < c_nChan ; ++i )
    {
      std::ifstream* pCur = m_pFiles->mf_Get( i );
      pCur->close();
      delete pCur;
    }
    ZeroizingDeleter()( m_pFiles );
    mv_nSamples = 0;
    return true;
  }
  return false;
}

unsigned long BinaryInputFile::Read( void** a_pBuf, const unsigned ac_nSamples )
{
  if( !m_pFiles )
    return 0L;
  if( mv_nReadOffset >= mv_nSamples )
    return 0L;

  StreamType** pBuf = (StreamType**) a_pBuf;

  const unsigned nChan = mf_nChannels();
  unsigned long nRead = 0;

  for( unsigned i = 0 ; i < nChan ; ++i )
  {
    std::istream* pCur = m_pFiles->mf_Get( i );
    pCur->read( (char*) ( pBuf[ i ] ), ac_nSamples * sc_nSampleSize );
    nRead = pCur->gcount() / sc_nSampleSize;
  }

  mv_nReadOffset += nRead;

  return nRead;
}

unsigned BinaryInputFile::mf_nChannels() const
{
  if( m_pFiles )
    return m_pFiles->mf_nGetBufferSize();
  return 0;
}

unsigned long BinaryInputFile::mf_lSamplesLeft() const
{
  return mv_nSamples - mv_nReadOffset;
}

unsigned long BinaryInputFile::mf_lTotalSamples() const
{
  return mv_nSamples;
}

unsigned long BinaryInputFile::mf_lCurrentPosition() const
{
  return mv_nReadOffset;
}

void BinaryInputFile::mp_SeekPosition( const unsigned long ac_nPosition )
{
  if( ac_nPosition >= mf_lTotalSamples() )
    throw utils::StringException( "mp_SeekPosition got out of range!!" );

    //convert nSamples to bytes
  const unsigned long nToAdvance = ac_nPosition * sc_nSampleSize;

  const unsigned c_nChan = mf_nChannels();
  for( unsigned i = 0 ; i < c_nChan ; ++i )
  {
    std::ifstream* pCur = m_pFiles->mf_Get( i );
    pCur->clear( std::ios::goodbit );
    pCur->seekg( nToAdvance );
  }

    //update
  mv_nReadOffset = ac_nPosition;
}
