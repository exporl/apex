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
 
#ifndef __SOUNDCARD_H_
#define __SOUNDCARD_H_

#include <string>
#include <iostream> //FIXME remove
#include "defines.h"
#include "audioformat.h"
#include "utils/vectorutils.h"
#include "callback/callbackrunner.h"

namespace streamapp
{
  class AudioFormatReader;
  class AudioFormatWriter;

    /**
      * tSoundCardInfo
      *   struct describing soundcard specs.
      ************************************** */
  struct tSoundCardInfo
  {
    tSoundCardInfo() :
      m_nMaxInputChannels( 0 ),
      m_nMaxOutputChannels( 0 )
    {
    }
    
    unsigned                    m_nMaxInputChannels;
    unsigned                    m_nMaxOutputChannels;
    unsigned                    m_nDefaultBufferSize;
    std::vector<unsigned>       m_BufferSizes;
    std::vector<unsigned long>  m_SampleRates;

      /**
        * Check if the card supports this number of inputs.
        * @param ac_nChannels the number to query
        * @return true if the soundacrd can do it
        */
    INLINE bool mf_bCanInputChannels( const unsigned ac_nChannels ) const
    {
      return ac_nChannels <= m_nMaxInputChannels;
    }

      /**
        * Check if the card supports this number of outputs.
        * @param ac_nChannels the number to query
        * @return true if the soundacrd can do it
        */
    INLINE bool mf_bCanOutputChannels( const unsigned ac_nChannels ) const
    {
        std::cout << "max channels: " << m_nMaxOutputChannels << std::endl;
      return ac_nChannels <= m_nMaxOutputChannels;
    }

      /**
        * Check if the card supports this buffersize.
        * @param ac_nBufferSize the number to query
        * @return true if the soundacrd can do it
        */
    INLINE bool mf_bCanBufferSize( const unsigned ac_nBufferSize ) const
    {
      return utils::f_bHasElement( m_BufferSizes, ac_nBufferSize );
    }

      /**
        * Check if the card supports this samplerate.
        * @param ac_lSampleRate the number to query
        * @return true if the soundacrd can do it
        */
    INLINE bool mf_bCanSampleRate( const unsigned long ac_lSampleRate ) const
    {
      return utils::f_bHasElement( m_SampleRates, ac_lSampleRate );
    }
  };

    /**
      * BaseSampleRates
      *   defines the base samplerates.
      *   Using multiplying, all samplerates commonly used
      *   by digital audio can be calculated.
      *   Used for querying SoundCard specs.
      **************************************************** */
  class BaseSampleRates : public std::vector<unsigned long>
  {
  public:
      /**
        * Constructor.
        * Initializes the vector.
        */
    BaseSampleRates() :
      sc_nMaxSampleRate( 192000 ),
      sc_dMaxSampleRate( 192000.0 )
    {
      std::vector<unsigned long>::push_back( 8000  );
      std::vector<unsigned long>::push_back( 11025 );
      std::vector<unsigned long>::push_back( 12000 );
    }

    const unsigned long  sc_nMaxSampleRate;   //
    const double         sc_dMaxSampleRate;   //should be sufficient for now

  private:
    BaseSampleRates( const BaseSampleRates& );
    BaseSampleRates& operator = ( const BaseSampleRates& );
  };

    /**
      * ISoundCard
      *   interface for soundcards.
      *   A soundcard is a device (mostly hardware) with the ability to
      *   peridically read and write blocks of audio samples.
      *   The periodicity translates into implementing CallbackRunner,
      *   the reading and writing into implementing AudioFormatReader and AudioFormatWriter.
      *   Most implementations will also provide a means to query available drivers
      *   via a static method.
      ************************************************************************************** */
  class ISoundCard : public CallbackRunner
  {
  protected:
      /**
        * Protected Constructor.
        * Doesn't open the device.
        */
    ISoundCard()
    {
    }

  public:
      /**
        * Destructor.
        */
    virtual ~ISoundCard()
    {
    }

      /**
        * Query specs.
        * @return a tSoundCardInfo filled with specs
        */
    virtual tSoundCardInfo  mf_GetInfo() const = 0;

      /**
        * Initialize the driver, set up all internal data structures.
        * The arguments better match the specs above,
        * else use mf_sGetLastError to see what's wrong.
        * @param ac_nIChan the number of input channels to use
        * @param ac_nOChan the number of output channels to use
        * @param ac_nFs the samplerate
        * @param ac_nBufferSize the buffersize
        * @return true if open
        * @see mf_sGetLastError()
        * @see mf_bIsOpen()
        */
    virtual bool            mp_bOpenDriver    ( const unsigned      ac_nIChan,
                                                const unsigned      ac_nOChan,
                                                const unsigned long ac_nFs,
                                                const unsigned      ac_nBufferSize ) = 0;

      /**
        * Close the driver.
        * @return false if it could not be closed, or if it wasn't open in the first place
        * @see mf_sGetLastError()
        * @see mf_bIsOpen()
        */
    virtual bool            mp_bCloseDriver   () = 0;

      /**
        * Get the number of input channels.
        * Use after mp_bOpenDriver()
        * @return the number
        */
    virtual unsigned        mf_nGetIChan      () const = 0;

      /**
        * Get the number of output channels.
        * Use after mp_bOpenDriver()
        * @return the number
        */
    virtual unsigned        mf_nGetOChan      () const = 0;

      /**
        * Get the number of samples (per channel) in every block.
        * Use after mp_bOpenDriver()
        * @return the number
        */
    virtual unsigned        mf_nGetBufferSize () const = 0;

      /**
        * Get the samplerate.
        * Use after mp_bOpenDriver()
        * @return the rate
        */
    virtual unsigned long   mf_lGetSampleRate () const = 0;

      /**
        * Check if the driver is opened.
        * @return true for open
        */
    virtual bool            mf_bIsOpen        () const = 0;

      /**
        * Get an estimation of the total latency between input and output.
        * This will be at least two times mf_nGetBufferSize() divided by mf_lGetSampleRate().
        * @return time in milliseconds
        */
    virtual unsigned long   mf_lGetEstimatedLatency () const = 0;

      /**
        * Implementation of the CallbackRunner method.
        */
    virtual bool            mp_bStart         ( Callback& a_CallbackToUse ) = 0;

      /**
        * Implementation of the CallbackRunner method.
        * The stop method must be implemented in a way that the card
        * won't stop when the callback is being executed.
        */
    virtual bool            mp_bStop          () = 0;

      /**
        * Implementation of the CallbackRunner method.
        */
    virtual bool            mf_bIsRunning     () const = 0;

      /**
        * Implementation of the CallbackRunner method.
        */
    INLINE virtual bool     mf_bIsBlocking() const
    {
      return false;
    }

      /**
        * Clear the input and output buffers on the soundcard (or in software).
        * Use to remove artefacts, some cards don't stop cleanly.
        */
    virtual void            mp_ClearIOBuffers() = 0;

      /**
        * Create an AudioFormatReader instance for this soundcard.
        * The reader must be able to alwys read samples,
        * whether or not the soundcard is running, although the samples
        * read are only updated every time the callback is called.
        * @return the reader or 0 for error
        */
    virtual AudioFormatReader*    mf_pCreateReader  () const = 0;

      /**
        * Create an AudioFormatWriter instance for this soundcard.
        * The reader must be able to alwys read samples,
        * whether or not the soundcard is running.
        * @return the reader or 0 for error
        */
    virtual AudioFormatWriter*    mf_pCreateWriter  () const = 0;

      /**
        * Get an error description.
        * @return a string reference
        * @see mp_bOpenDriver()
        * @see mp_bCloseDriver()
        */
    virtual const std::string&    mf_sGetLastError  () const = 0;
  };

}

#endif //#ifndef __SOUNDCARD_H_
