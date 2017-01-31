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

#ifndef __RMSTESTER_H__
#define __RMSTESTER_H__

#include "stream.h"
#include "containers/matrix.h"

namespace streamapp
{
  class Channel;
  class ISoundCard;
  class IOProcessor;

    /**
      * RmsProc
      *   calculates averaged rms.
      *   Every mf_DoProcessing() call, the rms of all channels
      *   is calculated and avaraged with the previous value.
      ********************************************************* */
  class RmsProc : public StreamProcessor
  {
  public:
      /**
        * Constructor.
        * @param ac_nChan number of channels to check
        * @param ac_nSize number of samples to check
        */
    RmsProc( const unsigned ac_nChan, const unsigned ac_nSize );

      /**
        * Destructor.
        */
    ~RmsProc();

      /**
        * Do processing.
        * @param ac_ToCheck the stream to process
        * @return ac_ToCheck unchanged
        */
    const Stream& mf_DoProcessing( const Stream& ac_ToCheck );

      /**
        * Reset the average.
        */
    void mp_Reset();

      /**
        * Get the average.
        * @return all rms levels
        */
    INLINE const StreamChannel& mf_GetLevels() const
    {
      return m_Rms;
    }

      /**
        * Get the average count.
        * @return number of buffers checked so far
        */
    INLINE const StreamType& mf_dAverageCount() const
    {
      return mv_dCount;
    }

  private:
    StreamChannelBuf  m_Rms;
    StreamType        mv_dCount;
    bool              mv_bReset;
  };


    /**
      * RmsTester
      *   plays a sine and records the signal
      *   and calculates averaged RMS levels
      *************************************** */
  class RmsTester
  {
  public:
      /**
        * Constructor.
        * @param a_pSoundCard the soundcard to use
        */
    RmsTester( ISoundCard* a_pSoundCard );

      /**
        * Destructor.
        * Will not delete the soundcard.
        */
    ~RmsTester();

      /**
        * Set the signal source to use for soundcard output
        * @param a_pStream the stream
        */
    void mp_SetSource( InputStream* a_pStream );

      /**
        * Start.
        */
    void mp_Start();

      /**
        * Stop.
        */
    void mp_Stop();

      /**
        * Get the recorded level.
        * @param ac_nChannel the channel number
        * @return rms level
        */
    StreamType mf_dGetRecordedRMS( const unsigned ac_nChannel ) const;

  private:
    ISoundCard*     m_pCard;
    IOProcessor*    m_pProc;
    Channel*        m_pCallback;
    RmsProc*        m_pRMS;
  };

}

#endif //#ifndef __RMSTESTER_H__
