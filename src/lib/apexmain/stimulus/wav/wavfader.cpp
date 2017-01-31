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

#include "wavfader.h"
#include <processors/gate.h>
#include <utils/dataconversion.h>

#include "filter/wavparameters.h"

using namespace apex;
using namespace apex::stimulus;
using namespace streamapp;

WavFader::WavFader ( const QString& ac_sID, data::WavFaderParameters* a_pParams,
                   unsigned long sr, unsigned bs) :
        WavFilter ( ac_sID, a_pParams, sr, bs ),
        m_pProc ( 0 ),
        m_Params ( a_pParams )
{
    mp_Init();
}

WavFader::~WavFader()
{
}

void WavFader::mp_Init()
{
    m_bFadeIn = m_Params->isFadeIn();
    mp_SetFadeLength ( m_Params->length() );
    switch ( m_Params->type() )
    {
        case data::WavFaderParameters::LINEAR :
            m_pProc = new Gate ( m_Params->numberOfChannels(), GetBlockSize(),
                                 m_bFadeIn ? tFadeFunc::smf_dLinearFadeIn : tFadeFunc::smf_dLinearFadeOut );
            break;
        case data::WavFaderParameters::COSINE :
            m_pProc = new Gate ( m_Params->numberOfChannels(), GetBlockSize(),
                                 m_bFadeIn ? tFadeFunc::smf_dCosineFadeIn : tFadeFunc::smf_dCosineFadeOut );
            break;
        default:
            Q_ASSERT ( "unknown type" );
    }
    m_pProc->setMutedAfterwards(!m_bFadeIn);
}

streamapp::IStreamProcessor* WavFader::GetStrProc() const
{
    Q_ASSERT ( m_pProc );
    return m_pProc;
}

bool WavFader::SetParameter ( const QString& type, const int channel, const QVariant& value )
{
    Q_UNUSED ( channel );
    if ( type == "length" )
    {
        mp_SetFadeLength ( value.toDouble() );
        return true;
    }
    return false;
}

void WavFader::Reset()
{
    //..and get back to original length
    mp_SetFadeLength ( m_Params->length() );
}

void WavFader::Prepare()
{
    //called before stream starts, so reset sample counter..
    m_pProc->mp_Reset();
}

void WavFader::mp_SetFadeLength ( const double ac_dMSec )
{
    qDebug ( "WavFader: setting fade length to %f", ac_dMSec);
    m_dFadeLength = dataconversion::gf_nSamplesFromMsec< unsigned > ( GetSampleRate(), ac_dMSec );
}

void WavFader::mp_SetStreamLength ( const unsigned long ac_nSamples )
{
    qDebug("Streamlength: %lu samples", ac_nSamples);
    if ( m_bFadeIn )
    {
        m_pProc->mp_SetGateOn ( 0 );
        m_pProc->mp_SetGateLength ( m_dFadeLength );
    }
    else
    {
        m_pProc->mp_SetGateOn ( ( double ) ac_nSamples - m_dFadeLength );
        m_pProc->mp_SetGateLength ( m_dFadeLength );
    }
}
