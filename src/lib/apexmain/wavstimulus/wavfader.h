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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_WAVFADER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_WAVFADER_H_

#include "wavfilter.h"

namespace streamapp
{
class Gate;
}

namespace apex
{
namespace data
{
class WavFaderParameters;
}
namespace stimulus
{

/**
  * WavFader
  *   processor for fading in/out.
  ******************************** */
class WavFader : public WavFilter
{
public:
    /**
      * Constructor.
      * @param ac_sID filter's ID
      * @param a_pParams parameters
      */
    WavFader(const QString &ac_sID, data::WavFaderParameters *a_pParams,
             unsigned long sr, unsigned bs);

    /**
      * Destructor.
      */
    ~WavFader();

    /**
      * Implementation of the WavFilter method.
      */
    INLINE bool mf_bIsRealFilter() const
    {
        return true;
    }

    /**
      * Implementation of the WavFilter method.
      */
    bool SetParameter(const QString &type, const int channel,
                      const QVariant &value);

    /**
      * Implementation of the WavFilter method.
      */
    INLINE void Reset();

    virtual void Prepare();

    /**
      * Implementation of the WavFilter method.
      */
    streamapp::IStreamProcessor *GetStrProc() const;

    /**
      * Implementation of the WavFilter method.
      */
    bool mf_bWantsToKnowStreamLength() const
    {
        return true;
    }

    /**
      * Implementation of the WavFilter method.
      */
    void mp_SetStreamLength(const unsigned long ac_nSamples);

private:
    void mp_Init();
    void mp_SetFadeLength(const double ac_dMSec);

    double m_dFadeLength;
    bool m_bFadeIn;
    streamapp::Gate *m_pProc;
    data::WavFaderParameters *m_Params;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_WAVSTIMULUS_WAVFADER_H_
