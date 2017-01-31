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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_IMIXER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_IMIXER_H_

#include "apexdata/factoryelement.h"

#include "device/controldevice.h"
#include "device/iapexdevice.h"

namespace apex
{
    namespace data {
           class MixerParameters;
    }

  namespace device
  {





      /**
        * IMixer
        *   simplified version of StreamApp's mixer interface (no matrix, only output).
        *   @see streamapp::IMixer
        *   Apex' mixer can get/set the gain of one or all channels
        *   of a hardware mixer/attenuator
        ******************************************************************************* */
    class IMixer : public ControlDevice
    {
    protected:
        /**
          * Constructor.
          * @param ac_sID a unique ID
          * @param a_pParameters the parameters, must not contain a parameter with ID "gain"
          * since the parameters with name "gain" is what we look for.
          */
        IMixer(data::MixerParameters* a_pParameters );
    public:
        /**
          * Destructor.
          */
      virtual ~IMixer();

        /**
          * Get the maximum gain allowed.
          * @return the gain in dB
          */
      virtual double mf_dGetMaxGain() const = 0;

        /**
          * Get the minimum gain possible.
          * @return the gain in dB
          */
      virtual double mf_dGetMinGain() const = 0;

        /**
          * Set the gain on all channels.
          * @param ac_dGainIndB gain in dB
          */
      virtual void mp_SetGain( const double ac_dGainIndB ) = 0;

        /**
          * Set the gain on the given channel.
          * Must be in range.
          * @param ac_dGainIndB gain in dB
          * @param ac_nChannel the channe;
          */
      virtual void mp_SetGain( const double ac_dGainIndB, const unsigned ac_nChannel ) = 0;

        /**
          * Get the gain for the given channel
          * @param ac_nChannel the channel number
          * @return the gain
          */
      virtual double mf_dGetGain( const unsigned ac_nChannel ) const = 0;

        /**
          * Get the number of channels for the mixer.
          * @return the number
          */
      virtual unsigned mf_nGetNumChannels() const = 0;

        /**
          * Implementation of the IApexDevice method.
          * The default implementation calls mp_SetGain for all channels if ac_sParameter == "gain",
          * or for the channel(s) specified by ac_sParameter as ID,
          * and checks whether the gain is in range; if not, query GetInfo with mc_eOutOfRange
          * @param ac_sParameter the name ("gain") or the ID to set
          * @param ac_sValue the new gain value
          * @return true if succeedes
          */
      //virtual bool SetParameter( const QString& ac_sParameter, const QString& ac_sValue );
      virtual bool SetParameter ( const QString& type, const int channel, const QVariant& value ) ;

        /**
          * Implementation of the IApexDevice method.
          * Type should be mc_eOutOfRange. If SetParameter() was called
          * with a gain that was out of range, the method returns true
          * and a_pInfo points to a QString containing the error message.
          */
      virtual bool GetInfo( const unsigned ac_nType, void* a_pInfo ) const;


      virtual void Reset();
      virtual void Prepare();

    private:
      double mf_dGetGainInRange( const double ac_dGain );

      mutable QString mv_sInfo;
    };

  }
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_IMIXER_H_
