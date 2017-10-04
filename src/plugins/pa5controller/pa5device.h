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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_PA5DEVICE_PA5DEVICE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_PA5DEVICE_PA5DEVICE_H_

#ifdef PA5

#include "device/mixer/imixer.h"

class Pa5ActX;
class QString;

namespace apex
{
namespace data
{
class MixerParameters;
}
namespace device
{

/**
  * Pa5Device
  *   TDT's Pa5 attenuator abstraction.
  *   @TODO implement multiple PA5s.
  ************************************* */
class Pa5Device : public IMixer
{
public:
    /**
      * Constructor.
      * @param ac_sID unique ID
      * @param a_pParameters the default initial parameters
      */
    Pa5Device(data::MixerParameters *a_pParameters);

    /**
      * Destructor.
      */
    ~Pa5Device();

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
      * Note that Pa5 itself sets attenuation instead of amplification,
      * so the ac_dGainIndB is inverted here.
      * @param ac_dGainIndB the attenuation in dB
      */
    void mp_SetGain(const double ac_dGainIndB);

    /**
      * Implementation of the IMixer method.
      * @see mp_SetGain( const double ac_dGainIndB )
      */
    void mp_SetGain(const double ac_dGainIndB, const unsigned ac_nChannel);

    /**
      * Implementation of the IMixer method.
      */
    double mf_dGetGain(const unsigned ac_nChannel) const;

    /**
      * Implementation of the IMixer method.
      */
    unsigned mf_nGetNumChannels() const
    {
        return m_nNumPA5s;
    }

private:
    Pa5ActX *m_pActXObj;
    unsigned m_nNumPA5s;
};
}
}

#endif //#ifdef PA5
#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_PA5DEVICE_PA5DEVICE_H_
