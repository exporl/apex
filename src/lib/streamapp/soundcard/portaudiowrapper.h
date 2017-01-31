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

#ifndef PAWRAPPER_H
#define PAWRAPPER_H

#include "../typedefs.h"

#include "soundcard.h"
#include <QString>


Q_DECLARE_LOGGING_CATEGORY(soundcard)

typedef void  PaStream;
typedef int   PaDeviceIndex;

namespace streamapp
{

  class AudioFormatReader;
  class AudioFormatWriter;
  class PortAudioReader;
  class PortAudioWriter;
  struct PaUserData;

    /**
      * PortAudioWrapper
      *   ISoundCard implementation with PortAudio.
      *   Depending on the library used, this can be DirectX, WinMME, Oss etc.
      *   @note PortAudio always uses at least 4 buffers no matter what it's
      *   asked to do. This means if you put buffersize to 1024, the latency
      *   will be somewhere near 2 * 1024 * 4 divided by samplerate.
      *   NOTE problems for OsX:
      *   - opening a card with PaInt32 gives no errors, yet there's no sound
      *     with onboard soundcard, so we open it as PaFloat and do extra
      *     conversions
      *   - standard portaudio implementation has a *horrible* feature: it
      *     automatically raises the volume to maximum.. bad for your ears,
      *     so fix the source first!
      *   - numSampleRates always is -1 ,even though the samplerate array
      *     cotains all values; so we just push back some common rates and
      *     hope they match the soundcard's abilities.
      *   - when opening MAudio Audiophile at 44.1kHz, the buffers are not
      *     cleared properly and the output gets messed up
      *   @see mp_SetNumBuffers()
      *   @see mf_lGetEstimatedLatency()
      *   @see mf_nGetActualBufferSizeUsed()
      ************************************************************************ */
  class PortAudioWrapper : public ISoundCard
  {
  public:
      /**
        * Constructor.
        * @param ac_sDriverName the drivername to use
        */
    PortAudioWrapper( const QString& cardName = sc_sDefault );

      /**
        * Destructor.
        */
    ~PortAudioWrapper();

      /**
        * Get the names of all available drivers.
        * @param a_sError receives possible error
        * @return vector of names
        */
    static tStringVector sf_saGetDriverNames(QString &a_sError );
    static tStringVector sf_saGetDriverNames(std::string &a_sError );

      /**
        * Implementation of the ISoundCard method.
        */
    tSoundCardInfo  mf_GetInfo() const;

      /**
        * Implementation of the ISoundCard method.
        */
    bool            mp_bOpenDriver    ( const unsigned      ac_nIChan,
                                        const unsigned      ac_nOChan,
                                        const unsigned long ac_nFs,
                                        const unsigned      ac_nBufferSize );

      /**
        * Implementation of the ISoundCard method.
        */
    bool            mp_bCloseDriver   ();

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE unsigned         mf_nGetIChan      () const;

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE unsigned         mf_nGetOChan      () const;

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE unsigned         mf_nGetBufferSize   () const;

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE unsigned long    mf_lGetSampleRate () const;

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE AudioFormat::mt_eBitMode mf_eGetBitMode() const;

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE bool             mf_bIsOpen        () const
    {
      return mv_bOpen;
    }

      /**
        * Implementation of the ISoundCard method.
        */
    unsigned long           mf_lGetEstimatedLatency() const;

      /**
        * Implementation of the ISoundCard method.
        */
    bool                    mp_bStart         ( Callback& a_CallbackToUse );

      /**
        * Implementation of the ISoundCard method.
        */
    bool                    mp_bStop          ();

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE bool             mf_bIsRunning     () const;

      /**
        * Implementation of the ISoundCard method.
        */
    AudioFormatReader*      mf_pCreateReader() const;

      /**
        * Implementation of the ISoundCard method.
        */
    AudioFormatWriter*      mf_pCreateWriter() const;

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE const std::string& mf_sGetLastError() const
    {
      return mv_sError;
    }

      /**
        * Implementation of the ISoundCard method.
        * Doesn't do anything, Portaudio's buffers are not accessible when it's not running
        */
    INLINE void mp_ClearIOBuffers()
    {
    }

      /**
        * Get a pointer to the PortAudioStream object.
        * @return the pointer
        */
    INLINE const PaStream* mf_pGetPaStream() const
    {
      return m_pPaStream;
    }

    friend class PortAudioReader;
    friend class PortAudioWriter;

  private:
    PaStream*                 m_pPaStream;
    PaUserData*               m_pUserData;
    bool                      mv_bOpen;
    unsigned                  mv_nBuffers;
    std::string               mv_sError;
    PaDeviceIndex             m_iInputID;
    PaDeviceIndex             m_iOutputID;
  };

}

#endif //PAWRAPPER_H
