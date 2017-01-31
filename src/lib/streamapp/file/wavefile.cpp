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

#include "file/wavefile.h"
#include "utils/stringexception.h"
#include "utils/stringutils.h"
#include "utils/dataconversion.h"
#include "utils/tracer.h"
#include <core/containers/looplogic.h>
#include "defines.h"
#include <assert.h>
#include <iostream>
#include <cstring>

using namespace std;
using namespace dataconversion;

using namespace streamapp;

InputWaveFile::InputWaveFile() :
  m_lTotalSamples( 0l ),
  m_pif( 0 ),
  m_pLoop( new mt_LoopLogic( m_lTotalSamples, mv_lLoopStart, mv_lLoopEnd, m_nReadOffset ) )
{
}

InputWaveFile::~InputWaveFile()
{
  mp_bClose();
  delete m_pLoop;
}

bool InputWaveFile::mp_bOpen( const std::string& ac_sFileName )
{
    //do not allow open more than once
  if( !m_pif )
  {
    m_pif = new ifstream( ac_sFileName.data(), ios::in|ios::binary );
    if( !m_pif->is_open() )
    {
      delete m_pif;
      m_pif = 0;
      return false;
    }

    try
    {
      ReadRIFF();
      ReadFMT();
      ReadDATA();
    }
    catch( ... )
    {
      throw( utils::StringException( "bad wavefile header" ) );
    }

    m_nDataLeft         = m_pDATA->dataSIZE;

    const short wFormat = m_pFMT->fmtFORMAT.wFormatTag;
    const short wBitsPS = m_pFMT->fmtFORMAT.wBitsPerSample;

    if( wBitsPS == 16 && wFormat == 1 )
    {
      m_eBitMode = AudioFormat::MSBint16;
      m_nBytesPerSample = 2;
    }
    else if( wBitsPS == 32 && wFormat == 1 )
    {
      m_eBitMode = AudioFormat::MSBint32;
      m_nBytesPerSample = 4;
    }
    else if( wBitsPS == 32 && wFormat == 3 )
    {
      m_eBitMode = AudioFormat::MSBfloat32;
      m_nBytesPerSample = 4;
    }
    else if( wBitsPS == 24 && wFormat == 1 )
    {
      m_eBitMode = AudioFormat::MSBint24;
      m_nBytesPerSample = 3;
    }
    else
    {
      throw( utils::StringException( "unknown wave format" ) );
    }
    m_nBytesPerFrame  = mf_nChannels() * m_nBytesPerSample;
    m_lTotalSamples   = m_pDATA->dataSIZE / m_nBytesPerFrame;


      //some wave editors append extra bytes after the samples,
      //we have to check this here since we count up instead of down
      //new: just force correct file format instead of checking it every time
    std::ifstream::pos_type cur = m_pif->tellg();   //code from gf_nStreamLength in stl/fileutils.h
    m_pif->seekg( 0 );
    std::ifstream::pos_type begin = m_pif->tellg();
    m_pif->seekg( 0, std::ios::end );
    std::ifstream::pos_type end = m_pif->tellg();
    m_pif->seekg( cur );
    long len = end - begin;
    long dsize = len - WaveFileData::sf_lHeaderSize;
    if( (unsigned long ) dsize > m_lTotalSamples * m_nBytesPerFrame ) {
      DBGPF( "InputWaveFile: file has %d bytes too many",
              dsize - m_lTotalSamples * m_nBytesPerFrame);
    }

    return true;
  }
  return false;
}

bool InputWaveFile::mp_bClose()
{
  if( m_pif && m_pif->is_open() )
  {
    m_pif->close();
    delete m_pif;
    m_pif = 0;
    return true;
  }
  return false;
}

unsigned long InputWaveFile::Read( void** a_pBuf, const unsigned ac_nSamples )
{
  if( !m_pif->is_open() )
    throw( 0 );  //should never come here

  const bool bLoop = mf_bIsLooping();
  if( !bLoop && m_nReadOffset >= m_lTotalSamples )
    return 0l;    //eof

  unsigned nSamples  = ac_nSamples;
  unsigned nRead     = 0;

  int* dest0 = ( (int**) a_pBuf )[ 0 ];
  int* dest1 = mf_nChannels() < 2 ? NULL : ( (int**) a_pBuf )[ 1 ];
  //int* dest1 = ( (int**) a_pBuf )[ 1 ];

  while( nSamples > 0 )
  {
    const unsigned nThisTime  = s_min( tempBufSize / m_nBytesPerFrame, nSamples );
    unsigned nBytesToRead     = nThisTime * m_nBytesPerFrame;
    unsigned nSamplesRead     = 0;

    if( !bLoop )
    {
      m_pif->read( tempBuffer,
              qMin(mf_lSamplesLeft() * m_nBytesPerFrame, (unsigned long)nBytesToRead));
      nSamplesRead = m_pif->gcount() / m_nBytesPerFrame;
      m_nReadOffset += nSamplesRead;
    }
    else
    {
      unsigned nLeft = nThisTime - nSamplesRead;
      while( nLeft )
      {
        unsigned nLoopRead = m_pLoop->mf_nGetPiece( nLeft );

        m_pif->read( tempBuffer + ( nSamplesRead * m_nBytesPerFrame ),
              qMin(mf_lSamplesLeft() * m_nBytesPerFrame, nLoopRead * m_nBytesPerFrame));

        unsigned nReadLoop = m_pif->gcount() / m_nBytesPerFrame;
        nSamplesRead += nReadLoop;
        nLeft = nThisTime - nSamplesRead;

        mp_SeekPosition( m_pLoop->mf_nGetNewPos( nReadLoop ) );
      }
    }

    nRead += nSamplesRead;

      //convert all nSamplesRead
    mf_Convert( dest0, dest1, nSamplesRead );

      //no need to try further
    if( nSamplesRead != nThisTime )
    {
      m_pif->clear();
      return nRead;
    }

    nSamples -= nThisTime;
  }
  return nRead;
}

void InputWaveFile::mf_Convert( int*& dest0, int*& dest1, const unsigned nSamplesRead )
{
  const unsigned nChannels = mf_nChannels();

  if( m_eBitMode == AudioFormat::MSBint16 )
  {
    const short* src = (const short*) tempBuffer;

    if( nChannels == 1 )
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++ i )
        *dest0++ = *src++ << 16;
    }
    else
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++ i )
      {
        *dest0++ = *src++ << 16;
        *dest1++ = *src++ << 16;
      }
    }
  }
  else if( m_eBitMode == AudioFormat::MSBint24 )
  {
    const char* src = (const char*)tempBuffer;
    if( nChannels == 1 )
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++ i )
      {
        *dest0++ = littleEndian24Bit( src ) << 8;
        src += 3;
      }
    }
    else
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++ i )
      {
        *dest0++ = littleEndian24Bit( src ) << 8;
        src += 3;
        *dest1++ = littleEndian24Bit( src ) << 8;
        src += 3;
      }
    }
  }
  else if( m_eBitMode == AudioFormat::MSBint32 )
  {
    const int* src = (const int*)tempBuffer;
    if( nChannels == 1 )
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++i )
        *dest0++ = *src++;
    }
    else
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++i )
      {
        *dest0++ = *src++;
        *dest1++ = *src++;
      }
    }
  }
  else if( m_eBitMode == AudioFormat::MSBfloat32 )
  {
    const int* src = (const int*)tempBuffer;
    if( nChannels == 1 )
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++i )
        *dest0++ = *src++;
    }
    else
    {
      for( unsigned i = 0 ; i < nSamplesRead ; ++i )
      {
        *dest0++ = *src++;
        *dest1++ = *src++;
      }
    }
  }
}

INLINE unsigned long InputWaveFile::mf_lSamplesLeft() const
{
  return m_lTotalSamples - m_nReadOffset;
}

INLINE unsigned long InputWaveFile::mf_lTotalSamples() const
{
  return m_lTotalSamples;
}

INLINE unsigned long InputWaveFile::mf_lCurrentPosition() const
{
  return m_nReadOffset;
}

void InputWaveFile::mp_SeekPosition( const unsigned long ac_nPosition )
{
  if( ac_nPosition >= mf_lTotalSamples() )
    throw utils::StringException( "mp_SeekPosition got out of range!!" );
    //convert nSamples to bytes
  const unsigned long nToAdvance = ac_nPosition * m_nBytesPerFrame;
    //seek pos, don't forget to jump over header
  m_pif->clear();
  m_pif->seekg( nToAdvance + WaveFileData::sf_lHeaderSize );
    //update
  m_nReadOffset = ac_nPosition;
}
void InputWaveFile::ReadRIFF()
{
  m_pif->read((char*)m_pRIFF, sizeof(RIFF));
  if( !CheckID( m_pRIFF->riffID, 'R', 'I', 'F', 'F') ) {
    throw( 0 );
  }
  if( !CheckID( m_pRIFF->riffFORMAT, 'W', 'A', 'V', 'E') ) {
    throw( 0 );
  }
}
void InputWaveFile::ReadFMT()
{
  m_pif->read((char*)m_pFMT, sizeof(FMT));
  if( !CheckID( m_pFMT->fmtID, 'f', 'm', 't', ' ') ) {
    throw( 0 );
  }
}
void InputWaveFile::ReadDATA()
{
  try
  {
    m_pif->read( (char*) m_pDATA, sizeof( DATA ) );
    if( !CheckID( m_pDATA->dataID, 'd', 'a', 't', 'a') )
      throw( 0 );
  }
  catch( ... )
  {
      //something stange! In wave files the DATA identifier
      //can be offseted (maybe because of address aligment)
      //Start to looking DATA_ID "manualy" ;)
    m_pif->seekg( sizeof( RIFF ) + sizeof( FMT ) );
    bool foundData = false;
    char ch;

    while( m_pif )
    {
      m_pif->get(ch);
      if( ch == 'd' )
      {
          //It can be DATA_ID, check it!
        m_pif->unget();
        m_pif->read( (char*)m_pDATA, sizeof( DATA ) );
        if( CheckID( m_pDATA->dataID, 'd','a','t','a' ) )
        {
          foundData = true;
          break;
        }
      }
    }

    if( !foundData )
      throw( 0 );
  }
}

bool InputWaveFile::CheckID( char *idPar, char A, char B, char C, char D )
{
  return
  (
    ( idPar[ 0 ] == A ) &&
    ( idPar[ 1 ] == B ) &&
    ( idPar[ 2 ] == C ) &&
    ( idPar[ 3 ] == D )
  );
}

/************************************************************* OutputWaveFile *********************************************************/

OutputWaveFile::OutputWaveFile() :
  m_pof( 0 )
{
}

OutputWaveFile::~OutputWaveFile()
{
  mp_bClose();
}

bool OutputWaveFile::mp_bOpen( const std::string& ac_sFileName, const unsigned ac_nChannels, const unsigned long ac_nFs, const AudioFormat::mt_eBitMode ac_eMode )
{
    //do not allow create more than once
  if( !m_pof )
  {
    if( ac_nChannels == 0 || ac_nChannels > 2 || ac_nFs ==0 )
      return false;

    m_pof = new ofstream( ac_sFileName.data(), ios::out | ios::binary | ios::trunc );
    if( !m_pof->is_open() )
      return false;

    const short c_nChannels = (short)ac_nChannels;

    m_eBitMode = ac_eMode;
    short wBitsPS = 0;
    short wFormat = 0;
    short wBlockA = 0;
    if( m_eBitMode == AudioFormat::MSBint16 )
    {
      wBitsPS = 16;
      wFormat = 1;
      wBlockA = 2 * c_nChannels;
      m_nBytesPerSample = 2;
    }
    else if( m_eBitMode == AudioFormat::MSBint32 )
    {
      wBitsPS = 32;
      wFormat = 1;
      wBlockA = 4 * c_nChannels;
      m_nBytesPerSample = 4;
    }
    else if( m_eBitMode == AudioFormat::MSBfloat32 )
    {
      wBitsPS = 32;
      wFormat = 3;
      wBlockA = 4 * c_nChannels;
      m_nBytesPerSample = 4;
    }
    else if( m_eBitMode == AudioFormat::MSBint24 )
    {
      wBitsPS = 24;
      wFormat = 1;
      wBlockA = 3 * c_nChannels;
      m_nBytesPerSample = 3;
    }
    else
    {
      throw( utils::StringException( "unknown wave format" ) );
    }

    m_nBytesPerFrame = ac_nChannels * m_nBytesPerSample;

      //RIFF
    strncpy( m_pRIFF->riffID, "RIFF", 4 );
    m_pRIFF->riffSIZE = sf_lHeaderSize - 8;
    strncpy( m_pRIFF->riffFORMAT, "WAVE", 4 );

      //FMT
    strncpy( m_pFMT->fmtID, "fmt ", 4 );
    m_pFMT->fmtSIZE = sizeof( WAVEFORM );
    m_pFMT->fmtFORMAT.wFormatTag      = wFormat;
    m_pFMT->fmtFORMAT.nChannels       = (short) ac_nChannels;
    m_pFMT->fmtFORMAT.nSamplesPerSec  = ac_nFs;
    m_pFMT->fmtFORMAT.nBlockAlign     = wBlockA;
    m_pFMT->fmtFORMAT.nAvgBytesPerSec = m_nBytesPerFrame * ac_nFs;
    m_pFMT->fmtFORMAT.wBitsPerSample  = wBitsPS;

      //DATA
    strncpy( m_pDATA->dataID, "data", 4 );
    m_pDATA->dataSIZE = 0;

    m_pof->write( (char*)m_pRIFF, sizeof(RIFF) );
    m_pof->write( (char*)m_pFMT , sizeof(FMT)  );
    m_pof->write( (char*)m_pDATA, sizeof(DATA) );

    return true;
  }

  return false;
}

bool OutputWaveFile::mp_bClose()
{
  if( m_pof )
  {
    WriteHeader();
    m_pof->close();
    delete m_pof;
    m_pof = 0;
    return true;
  }
  return false;
}

void OutputWaveFile::WriteHeader()
{
  m_pof->seekp( 0 );
  m_pRIFF->riffSIZE += m_nDataLeft;
  m_pDATA->dataSIZE += m_nDataLeft;
  m_pof->write( (char*)m_pRIFF, sizeof(RIFF) );
  m_pof->write( (char*)m_pFMT , sizeof(FMT)  );
  m_pof->write( (char*)m_pDATA, sizeof(DATA) );
}

unsigned long OutputWaveFile::Write( const void** a_pBuf, const unsigned ac_nSamples )
{
  if( !m_pof->is_open() )
    throw( utils::StringException( "OutputWaveFile::Write" ) );  //should *never* come here

  unsigned        nSamples = ac_nSamples;
  const unsigned nChannels = mf_nChannels();

  const int* src0 = (const int*) a_pBuf[ 0 ];
  const int* src1 = (const int*) a_pBuf[ 1 ];

  while( nSamples > 0 )
  {
    const unsigned nThisTime      = s_min( tempBufSize / m_nBytesPerFrame, nSamples );
    const unsigned nToWrite       = nThisTime * m_nBytesPerFrame;

    if( m_eBitMode == AudioFormat::MSBint16 )
    {
      short* dest = (short*) tempBuffer;
      if( nChannels == 1 )
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
          *dest++ = (short) ( *src0++ >> 16 );
      }
      else
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
        {
          *dest++ = (short) ( *src0++ >> 16 );
          *dest++ = (short) ( *src1++ >> 16 );
        }
      }
    }
    else if( m_eBitMode == AudioFormat::MSBint24 )
    {
      char* dest = (char*) tempBuffer;
      if( nChannels == 1 )
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
        {
          littleEndian24BitToChars( *src0++ >> 8, dest );
          dest += 3;
        }
      }
      else
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
        {
          littleEndian24BitToChars( *src0++ >> 8, dest );
          dest += 3;
          littleEndian24BitToChars( *src1++ >> 8, dest );
          dest += 3;
        }
      }
    }
    else if( m_eBitMode == AudioFormat::MSBint32 )
    {
      int* dest = (int*) tempBuffer;
      if( nChannels == 1 )
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
          *dest++ = *src0++;
      }
      else
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
        {
          *dest++ = *src0++;
          *dest++ = *src1++;
        }
      }
    }
    else if( m_eBitMode == AudioFormat::MSBfloat32 )
    {
      int* dest = (int*) tempBuffer;
      if( nChannels == 1 )
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
          *dest++ = *src0++;
      }
      else
      {
        for( unsigned i = 0 ; i < nThisTime ; ++ i )
        {
          *dest++ = *src0++;
          *dest++ = *src1++;
        }
      }
    }

    if( !m_pof->write( tempBuffer, nToWrite ) )
    {
        //disk full or so, write header so we still have usable data
      WriteHeader();
      return 0l;
    }

    m_nDataLeft += nToWrite;
    nSamples -= nThisTime;
  }

  return ac_nSamples;
}
