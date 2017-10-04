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

#ifndef __STR_FADER_H__
#define __STR_FADER_H__

#include "../utils/math.h"

#include "processor.h"

#include <QDebug>

namespace streamapp
{

/**
  * Gate
  *   simple gate functionality.
  *   A gate multiplies every sample at a given position
  *   with a gain. If the sample position is within the gate's
  *   range, the gain is taken from the gate func set.
  *   If mutedAfterwards is true, the signal will be muted after the gate.
  *   Else, the value is 1.0, hence nothing happens.
  ************************************************************* */
class Gate : public IGainProcessor
{
public:
    //! typedefs function to get gain; we only use simple stuff,
    //! else we could make a seperate class or use a template
    typedef double (*mt_FadeFunc)(const double &ac_dCurSample,
                                  const double &ac_dFadeLength);

    /**
      * Constructor.
      * @param ac_nChan
      * @param ac_nSize
      * @param ac_dLengthInSamples
      */
    Gate(const unsigned ac_nChan, const unsigned ac_nSize,
         mt_FadeFunc a_pfnFunc);

    /**
      * Destructor.
      */
    ~Gate();

    void setMutedAfterwards(bool value);
    bool isMutedAfterwards() const;

    /**
      * Set sample to trigger gate at.
      * @param ac_dSample sample number
      */
    void mp_SetGateOn(const double ac_dSample);

    /**
      * Set sample to turn gate off.
      * @param ac_dSample sample number, should be > gate on
      */
    void mp_SetGateOff(const double ac_dSample);

    /**
      * Set gate length.
      * gate off = gate on + gate len
      * @param ac_dSamples gate length in samples
      */
    void mp_SetGateLength(const double ac_dSamples);

    /**
      * Gate trigger position.
      * @return position in samples
      */
    const double &mf_dGateOn() const
    {
        return mv_dGateOn;
    }

    /**
      * Gate detrigger position.
      * @return position in samples
      */
    const double &mf_dGateOff() const
    {
        return mv_dGateOff;
    }

    /**
      * Gate length.
      * @return length in samples
      */
    const double &mf_dGateLength() const
    {
        return mv_dGateLength;
    }

    /**
      * Implementation of the IGainProcessor method.
      */
    double mf_dGetCurGain();

private:
    double mv_dGateOn;
    double mv_dGateOff;
    double mv_dGateLength;
    bool mutedAfterwards;
    mt_FadeFunc m_pfnGate;
};

/**
  * tFadeFunc
  *   provides fade in/out functions
  *   for use with @c Gate.
  ********************************** */
struct tFadeFunc {
    /**
      * Linear fadein.
      * @param ac_dCurSample sample position
      * @param ac_dFadeLength total fade length
      * @return ac_dCurSample / ac_dFadeLength
      */
    static double smf_dLinearFadeIn(const double &ac_dCurSample,
                                    const double &ac_dFadeLength)
    {
        return ac_dCurSample / ac_dFadeLength;
    }

    /**
      * Cosine shaped fadein.
      * @param ac_dCurSample sample position
      * @param ac_dFadeLength total fade length
      * @return 0.5 * ( 1.0 - cos( pi * ac_dCurSample / ac_dFadeLength ) )
      */
    static double smf_dCosineFadeIn(const double &ac_dCurSample,
                                    const double &ac_dFadeLength)
    {
        return 0.5 * (1.0 - cos(math::gc_dPi * ac_dCurSample / ac_dFadeLength));
    }

    /**
      * Linear fadeout.
      * @param ac_dCurSample sample position
      * @param ac_dFadeLength total fade length
      * @return 1.0 - ( ac_dCurSample / ac_dFadeLength )
      */
    static double smf_dLinearFadeOut(const double &ac_dCurSample,
                                     const double &ac_dFadeLength)
    {
        return 1.0 - (ac_dCurSample / ac_dFadeLength);
    }

    /**
      * Cosine shaped fadeout.
      * @param ac_dCurSample sample position
      * @param ac_dFadeLength total fade length
      * @return 0.5 * ( 1.0 - cos( pi * ( ac_dCurSample - ac_dFadeLength ) /
     * ac_dFadeLength ) )
      */
    static double smf_dCosineFadeOut(const double &ac_dCurSample,
                                     const double &ac_dFadeLength)
    {
        if (ac_dCurSample > ac_dFadeLength) {
            qCDebug(APEX_SA, "*");
            return 0;
        }
        return 0.5 *
               (1.0 - cos(math::gc_dPi * (ac_dCurSample - ac_dFadeLength) /
                          ac_dFadeLength));
    }
};
}

#endif //#ifndef __STR_FADER_H__
