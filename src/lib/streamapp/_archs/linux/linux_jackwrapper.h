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
 
#ifndef __JACKWRAPPER_H_
#define __JACKWRAPPER_H_

#include "typedefs.h"
#include "soundcard/soundcard.h"

namespace streamapp
{

  struct tJackUserData;

    /**
      * JackWrapper
      *   ISoundCard implementation for Jack system.
      *   JackWrapper represents a collection of connections to
      *   Jack input and/or output ports, for example if the server
      *   has
      *   alsa:builtin:In1, alsa:builtin:In2,
      *   alsa:builtin:Out1, alsa:builtin:Out2
      *   then opening a card with drivername alsa:builtin gives
      *   a 2channel full duplex audio streaming device.
      *
      *   @todo - check behaviour with multiple cards
      *         - check if buffersize/fs can change
      *         - use MSBFloat32 instead of doing xtra conversions
      *           (same for portaudio on IntelMac)
      *         - clean up a bit
      *         - use v3 and get rid of all memory copying ;-]
      *
      *   @note for IntelMac: you must use an "Aggregate Device"
      *   (see "AudioMIDISetup), else the driver fails to load.
      *
      ************************************************************* */
  class JackWrapper : public ISoundCard
  {
  public:
       /**
         * Constructor.
         * @param ac_sDrvName the name of the driver to open.
         */
     JackWrapper( const std::string& ac_sDrvName );

      /**
        * Destructor.
        * Closes any open driver.
        */
    ~JackWrapper();

      /**
        * Get the names of all available drivers.
        * @param a_sError receives possible errors
        * @return drivernames
        */
    static tStringVector sf_saGetDriverNames( std::string& a_sError );

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
    bool                    mp_bCloseDriver   ();

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
        * Most asio bitmodes are mapped to streamapp::eBitModes.
        */
    AudioFormat::mt_eBitMode mf_eGetBitMode    () const;

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
        * never* call start/stop from within the Callback!
        */
    bool                    mp_bStart         ( Callback& a_CallbackToUse );

      /**
        * Implementation of the ISoundCard method.
        */
    bool                    mp_bStop          ();

      /**
        * Implementation of the ISoundCard method.
        */
    bool                    mf_bIsRunning     () const;

      /**
        * Implementation of the ISoundCard method.
        */
    AudioFormatReader*      mf_pCreateReader  () const;

      /**
        * Implementation of the ISoundCard method.
        */
    AudioFormatWriter*      mf_pCreateWriter  () const;

      /**
        * Implementation of the ISoundCard method.
        */
    INLINE const std::string& mf_sGetLastError() const
    {
      return mv_sError;
    }

      /**
        * Implementation of the ISoundCard method.
        */
    void mp_ClearIOBuffers();

  private:
    const std::string mc_sDriver;
    tJackUserData*    m_pData;
    tStringVector     m_JackPortsIn;
    tStringVector     m_JackPortsOut;
    std::string       mv_sError;
    mutable bool      mv_bOpen;

    JackWrapper( const JackWrapper& );
    JackWrapper& operator = ( const JackWrapper& );
  };

}

#endif //#ifndef __JACKWRAPPER_H_
