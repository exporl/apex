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
 
#ifndef __COREADUDIOWRAPPER_H_
#define __COREADUDIOWRAPPER_H_

#include "typedefs.h"
#include "soundcard/soundcard.h"
#include <CoreAudio/AudioHardware.h>

namespace streamapp
{

  struct tCoreAudioUserData;

    //TODO should use this for all devices,
    //drivernames are not changed during prog runs normally
  struct tCoreAudioPersistentData
  {
    tStringVector                 m_Names;
    std::vector< AudioDeviceID >  m_IDs;
  };


  /**
    * CoreAudioWrapper
    *   ISoundCard implementation using CoreAudio framework.
    *   @note for the specs, only the number of channels can
    *    vary, the rest (buffersize&samplerate) is limited to
    *    the value set in the OS settings.
    *
    *   @todo use MSBfloat
    *
    ********************************************************* */
  class CoreAudioWrapper : public ISoundCard
  {
public:
      /**
        * Constructor.
        * @param ac_sDrvName the name of the driver to open.
        */
    CoreAudioWrapper( const std::string& ac_sDrvName );

      /**
        * Destructor.
        * Closes any open driver.
        */
    ~CoreAudioWrapper();

      /**
        * Get the names of all available drivers.
        * @return a string array
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
    AudioDeviceID       m_DevID;
    tCoreAudioUserData* m_pData;
    std::string         mv_sError;
    mutable bool        mv_bOpen;

    static tCoreAudioPersistentData sm_Data;

    CoreAudioWrapper( const CoreAudioWrapper& );
    CoreAudioWrapper& operator = ( const CoreAudioWrapper& );
  };

}

#endif //#ifndef __COREADUDIOWRAPPER_H_

