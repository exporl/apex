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

#ifndef __STR_BUFFEREDREADER_H__
#define __STR_BUFFEREDREADER_H__

#include "audioformat.h"
#include "containers/matrix.h"
#include "appcore/threads/locks.h"

namespace str3amapp
{
  namespace containers
  {
    template< class tType, class tCritSection >
    class CircularLogic;
  }
}
using namespace str3amapp;

namespace streamapp
{

  class Callback;

    /**
      * BufferedReader
      *
      * v3TODO: use Fifo or at least FifoLogic
      **************************************** */
  class BufferedReader : public PositionableAudioFormatReader
  {
  public:


    BufferedReader( PositionableAudioFormatReader* a_pParent,
                    const unsigned ac_nBufferSize,
                    const bool ac_bDeleteParent = true,
                    const bool ac_bRegisterWithThread = true );

      /**
        * Destructor.
        */
    virtual ~BufferedReader();

      /**
        * Implementation of the AudioFormat method.
        */
    virtual unsigned mf_nChannels() const
    {
      return m_pParent->mf_nChannels();
    }

      /**
        * Implementation of the AudioFormat method.
        */
    virtual unsigned long mf_lSampleRate() const
    {
      return m_pParent->mf_lSampleRate();
    }

      /**
        * Implementation of the AudioFormat method.
        */
    virtual AudioFormat::mt_eBitMode mf_eBitMode() const
    {
      return m_pParent->mf_eBitMode();
    }

      /**
        * Implementation of the AudioFormatReader method.
        */
    virtual unsigned long Read( void** a_pBuf, const unsigned ac_nSamples );

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    virtual unsigned long mf_lSamplesLeft() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    virtual unsigned long mf_lTotalSamples() const
    {
      return m_pParent->mf_lTotalSamples();
    }

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    virtual unsigned long mf_lCurrentPosition() const;

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    virtual void mp_SeekPosition( const unsigned long ac_lPosition );

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    virtual bool mp_bSetLoopStart( const unsigned long ac_lPosition )
    {
      if( m_pParent->mp_bSetLoopStart( ac_lPosition ) )
      {
        mv_lLoopStart = ac_lPosition;
        return true;
      }
      return false;
    }

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    virtual bool mp_bSetLoopEnd( const unsigned long ac_lPosition )
    {
      if( m_pParent->mp_bSetLoopStart( ac_lPosition ) )
      {
        mv_lLoopEnd = ac_lPosition;
        return true;
      }
      return false;
    }

      /**
        * Implementation of the PositionableAudioFormatReader method.
        */
    virtual void mp_SetLoopAll()
    {
      m_pParent->mp_SetLoopAll();
      mv_lLoopStart = 0;
      mv_lLoopEnd = mf_lTotalSamples();
    }


      /**
        * Get the buffer size.
        * @return size set in ctor
        */
    const unsigned long& mf_nBufferSize() const
    {
      return mc_nBufferSize;
    }

      /**
        * Fill the buffer.
        */
    unsigned long mp_FillBuffer();

      /**
        * Get the number of buffered samples.
        * @return samples currently in buffer
        */
    unsigned long mf_nGetNumInBuffer();

      /**
        * Wait until at least the given number of
        * samples is buffered. Always use this before
        * the first read or after changing the position,
        * else you risk a buffer underrun.
        * @param ac_lHowMuch requested number
        */
    void mf_WaitUntilFilled( const unsigned long ac_lHowMuch );

      /**
        * Install a Callback to call when
        * a buffer underrun occurs, ie when the number of
        * pre-buffered samples is less then the requested
        * read size. If this happens, increase buffersize
        * or change BufferThread timeout.
        * @param a_pCallback pointer to callback
        */
    void mp_InstallBufferDropCallback( Callback* a_pCallback )
    {
      m_pCallback = a_pCallback;
    }

  protected:
    struct mt_WriteOp
    {
      mt_WriteOp( PositionableAudioFormatReader* a_pReader,
                  const appcore::CriticalSection& ac_Lock );

      bool operator () ( int** a_pDest, const unsigned ac_nSamples );

    private:
      PositionableAudioFormatReader* m_pReader;
      const appcore::CriticalSection& mc_Lock;

      mt_WriteOp( const mt_WriteOp& );
      mt_WriteOp& operator = ( const mt_WriteOp& );
    };

    typedef containers::CircularLogic< int*, appcore::CriticalSection > mt_Fifo;

    const unsigned long            mc_nBufferSize;
    const bool                     mc_bDeleteParent;
    PositionableAudioFormatReader* m_pParent;

    unsigned long         mv_nPrevLeft;
    MatrixStorage< int >  m_Buffer;
    mt_Fifo*              m_pFifo;
    mt_WriteOp*           m_pBufferFiller;
    Callback*             m_pCallback;

    const appcore::CriticalSection mc_BigLock;
    const appcore::CriticalSection mc_SmallLock;
  };

}

//S_NAMESPACE_END

#endif //#ifndef __STR_BUFFEREDREADER_H__
