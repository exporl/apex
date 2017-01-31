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
 
#ifndef __LATENCYTESTER_H__
#define __LATENCYTESTER_H__

#include "connections.h"
#include "factorytypes.h"

namespace streamapp
{
  class OutputStream;
  template< bool >
  class MultiOutputStream;
  class PositionableInputStream;
  class ISoundCard;
  class EofCheckWaitCallback;

    /**
      * LatencyTester
      *   use to test soundcard latencies.
      *   Makes only sense for ASIO since that driver doesn't add extra buffers.
      *   With PORTAUDIO, you just know the total latency, not the soundcard-specific one.
      *   After mp_GaanMetDieBanaan() returns, there will be a number of files in
      *   ac_sOutputDir. Compare these to the inputfile, and you know the latency.
      ************************************************************************************ */
  class LatencyTester
  {
  public:
      /**
        * Constructor.
        * @param ac_nBufferSize the buffersize
        * @param ac_nChannels the number of channels
        * @param ac_nSamplerate the samplerate
        * @param ac_eType the soundcard type
        */
    LatencyTester( const unsigned ac_nBufferSize, const unsigned ac_nChannels, const unsigned long ac_nSamplerate, const gt_eDeviceType ac_eType = ASIO );

      /**
        * Destructor.
        */
    ~LatencyTester();

      /**
        * Set the input and output file names.
        * The usage is simple: these two must be set once prior to starting in the order they appear here.
        * @param ac_sInputFile the input file, must be mono
        * @param ac_sOutputDir the output directory for recorded files.
        */
    void mp_SetDiskIO( const std::string& ac_sInputFile, const std::string& ac_sOutputDir );

      /**
        * Set which input.and output channels to use.
        * @param ac_PlayChannelNumbers vector with numbers of channels to play
        * @param ac_RecordChannelNumbers vector with numbers of channels to record
        */
    void mp_SetChannels( const tUnsignedVector& ac_PlayChannelNumbers, const tUnsignedVector& ac_RecordChannelNumbers );

      /**
        * Play and record until input file is read.
        */
    void mp_GaanMetDieBanaan();

  private:
    std::string                   mv_sOutputDir;
    ISoundCard* const             mc_pCard;
    ConnectionManager             m_ConnMan;
    InputStream*                  m_pSoundInput;
    OutputStream*                 m_pSoundOutput;
    PositionableInputStream*      m_pInput;
    MultiOutputStream< true >*    m_pOutputs;
    EofCheckWaitCallback*         m_pCallback;

    LatencyTester& LabelEdit( const LatencyTester& );
    LatencyTester& operator = ( const LatencyTester& );
  };

}

#endif //#ifndef __LATENCYTESTER_H__
