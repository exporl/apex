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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_STREAMAPPMIXER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_STREAMAPPMIXER_H_

#include "imixer.h"

namespace streamapp
{
  class IMixer;
}

namespace apex
{
  namespace device
  {
      /**
        * StreamAppMixer
        *   translates StreamApp's mixer into Apex' IMixer interface.
        ************************************************************* */
    class StreamAppMixer : public IMixer
    {
    public:
        /**
          * Constructor.
          */
        StreamAppMixer( data::MixerParameters* a_pParameters );

        /**
          * Destructor.
          */
      ~StreamAppMixer();

        /**
          * Implementation of the IMixer method.
          */
      double mf_dGetMaxGain() const;

        /**
          * Implementation of the IMixer method.
          */
      double mf_dGetMinGain() const;

        /**
          * Implementation of the IMixer method.
          */
      void mp_SetGain( const double ac_dGainIndB );

        /**
          * Implementation of the IMixer method.
          */
      void mp_SetGain( const double ac_dGainIndB, const unsigned ac_nChannel );

        /**
          * Implementation of the IMixer method.
          */
      double mf_dGetGain( const unsigned ac_nChannel ) const;

        /**
          * Implementation of the IMixer method.
          */
      unsigned mf_nGetNumChannels() const;

    private:
      streamapp::IMixer* m_pMixer;
    };

  }
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_STREAMAPPMIXER_H_
