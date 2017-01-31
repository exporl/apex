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
 
#include "bufferedprocessor.h"
#include "utils/tracer.h"
#include "core/containers/circularlogic.h"
#include "callback/callback.h"


namespace streamapp
{

    /**
      * BufferReader
      ************** */
  class BufferReader : public InputStream
  {
  public:
      /**
        * Constructor.
        * @param a_pParent 
        */
    BufferReader( BufferedProcessing* a_pParent ) :
      m_Read( a_pParent->mc_nChannels, a_pParent->mc_nOutputBufferSize ),
      mc_pParent( a_pParent )
    {
    }

      /**
        * Destructor.
        */
    ~BufferReader()
    {
    }

      /**
        * Implementation of the InputStream method.
        */
    const Stream& Read()
    {
      unsigned nRead;
      mc_pParent->m_pFifo->mp_Read( m_Read.mf_pGetArray(), nRead, mc_pParent->mc_nOutputBufferSize );
      if( nRead != mc_pParent->mc_nOutputBufferSize )
      {
        DBG( "BufferReader: buffer underrun" );
        if( mc_pParent->m_pUnderRun )
          mc_pParent->m_pUnderRun->mf_Callback();
      }
      return m_Read;
    }

      /**
        * Implementation of the InputStream method.
        */
    unsigned mf_nGetNumChannels() const
    {
      return mc_pParent->mc_nChannels;
    }

      /**
        * Implementation of the InputStream method.
        */
    unsigned mf_nGetBufferSize() const
    {
      return mc_pParent->mc_nOutputBufferSize;
    }

  private:
    StreamBuf m_Read; //must keep copy since source is circular
    BufferedProcessing* const mc_pParent;

    BufferReader( const BufferReader& );
    BufferReader& operator = ( const BufferReader& );
  };

    /**
      * BufferWriter
      ************** */
  class BufferWriter : public OutputStream
  {
  public:
      /**
        * Constructor.
        * @param a_pParent 
        */
    BufferWriter( BufferedProcessing* a_pParent ) :
      mc_pParent( a_pParent )
    {
    }

      /**
        * Destructor.
        */
    ~BufferWriter()
    {
    }

      /**
        * Implementation of the OutputStream method.
        */
    void Write( const Stream& ac_Input )
    {
      Q_ASSERT( ac_Input.mf_nGetBufferSize() == mc_pParent->mc_nInputBufferSize );
      Q_ASSERT( ac_Input.mf_nGetChannelCount() == mc_pParent->mc_nChannels );
      unsigned nWritten;
      mc_pParent->m_pFifo->mp_Write( ac_Input.mf_pGetArray(), nWritten, mc_pParent->mc_nInputBufferSize );
      if( nWritten != mc_pParent->mc_nInputBufferSize )
      {
        DBG( "BufferWriter: buffer overrun" );
        if( mc_pParent->m_pOverRun )
          mc_pParent->m_pOverRun->mf_Callback();
      }
    }

      /**
        * Implementation of the OutputStream method.
        */
    unsigned mf_nGetNumChannels() const
    {
      return mc_pParent->mc_nChannels;
    }

      /**
        * Implementation of the OutputStream method.
        */
    unsigned mf_nGetBufferSize() const
    {
      return mc_pParent->mc_nInputBufferSize;
    }

  private:
    BufferedProcessing* const mc_pParent;

    BufferWriter( const BufferWriter& );
    BufferWriter& operator = ( const BufferWriter& );
  };
}

using namespace streamapp;

BufferedProcessing::BufferedProcessing( const unsigned ac_nInputBufferSize,
                                        const unsigned ac_nOutputBufferSize,
                                        const unsigned ac_nChannels,
                                        const unsigned ac_nBufferSize ) :
  mc_nInputBufferSize( ac_nInputBufferSize ),
  mc_nOutputBufferSize( ac_nOutputBufferSize ),
  mc_nChannels( ac_nChannels ),
  mc_nBufferSize( ac_nBufferSize ),
  m_Buffer( ac_nChannels, ac_nBufferSize ),
  m_pOverRun( 0 ),
  m_pUnderRun( 0 )
{
  m_pFifo = new mt_Fifo( m_Buffer.mf_pGetArray(), ac_nBufferSize, ac_nChannels );
  m_pReader = new BufferReader( this );
  m_pWriter = new BufferWriter( this );
}

BufferedProcessing::~BufferedProcessing()
{
  delete m_pWriter;
  delete m_pReader;
  delete m_pFifo;
}

InputStream& BufferedProcessing::mf_GetInputStream()
{
  return *m_pReader;
}

OutputStream& BufferedProcessing::mf_GetOutputStream()
{
  return *m_pWriter;
}

void BufferedProcessing::mp_Reset()
{
  m_pFifo->mp_SetEmpty();
}

unsigned BufferedProcessing::mf_nGetNumInBuffer() const
{
  return m_pFifo->mf_nGetMaxRead();
}

unsigned BufferedProcessing::mf_nGetNumFree() const
{
  return m_pFifo->mf_nGetMaxWrite();
}
