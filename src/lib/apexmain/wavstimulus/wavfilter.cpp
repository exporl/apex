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

#include "apexdata/filter/wavparameters.h"

#include "streamapp/appcore/threads/locks.h"

#include "streamapp/audiosamplebuffer.h"

#include "streamapp/processors/processor.h"
#include "streamapp/processors/processor.h"
#include "streamapp/processors/processor.h"

#include "streamapp/stream.h"

#include "streamapp/utils/dataconversion.h"

#include "streamgenerator.h"
#include "wavfilter.h"

using namespace dataconversion;
using namespace streamapp;
using namespace appcore;
using namespace apex;
using namespace apex::stimulus;

//! WavFilter

void WavFilter::SetBlockSize(unsigned bs)
{
    blockSize = bs;
}

unsigned WavFilter::GetBlockSize()
{
    return blockSize;
}

void WavFilter::SetSampleRate(unsigned long sr)
{
    sampleRate = sr;
}

unsigned long WavFilter::GetSampleRate()
{
    return sampleRate;
}

bool WavFilter::SetParameter(const QString &type, const int channel,
                             const QVariant &value)
{
    Q_UNUSED(type);
    Q_UNUSED(channel);
    Q_UNUSED(value);
    return true;
}

bool WavFilter::mf_bIsRealFilter() const
{
    return false;
}

//! WavAmplifier
WavAmplifier::WavAmplifier(const QString &ac_sID,
                           data::FilterData *const ac_pParams, unsigned long sr,
                           unsigned bs)
    : WavFilter(ac_sID, ac_pParams, sr, bs)
{
    data::WavFilterParameters *params = (data::WavFilterParameters *)ac_pParams;
    m_StrProc =
        new Amplifier(params->numberOfChannels(), GetBlockSize(), false);

    RestoreParameters();
}

WavAmplifier::~WavAmplifier()
{
}

bool WavAmplifier::SetParameter(const QString &type, const int channel,
                                const QVariant &value)
{
    if (type == "gain") {
        const double dGain =
            ((data::WavFilterParameters *)m_Params)->baseGain() +
            (((data::WavFilterParameters *)m_Params)->invertGain()
                 ? -value.toDouble()
                 : +value.toDouble());

        //#ifdef PRINTWAVFILTER
        qCDebug(APEX_RS, "new gain value for %s: %f", qPrintable(GetID()),
                dGain);
        //#endif

        if (channel != -1)
            m_StrProc->mp_SetGain(channel, dGain);
        else
            m_StrProc->mp_SetGain(dGain);

        mp_NeedRestore();
        return true;
    }
    return false;
}

void WavAmplifier::Reset()
{
    //    qCDebug(APEX_RS, "resetting gain for %s to %f", qPrintable (GetID()),
    //    0.0);
    m_StrProc->mp_SetGain(0);
}

IStreamProcessor *WavAmplifier::GetStrProc() const
{
    return m_StrProc;
}
