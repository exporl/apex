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

#include "apexdata/filter/filterdata.h"
#include "apexdata/filter/wavparameters.h"

#include "apextools/apextypedefs.h"

#include "stimulus/filtertypes.h"

#include "wavstimulus/streamgenerator.h"

#include "wavgenerator.h"

#include <QtGlobal>

using namespace apex::stimulus;

//! WavGenerator
apex::stimulus::WavGenerator::WavGenerator(const QString &ac_sID,
                                           const QString &ac_sType,
                                           data::FilterData *pParams,
                                           unsigned long sr, unsigned bs,
                                           bool deterministic)
    : WavFilter(ac_sID, pParams, sr, bs),
      m_pStrGen(StreamGeneratorFactory::CreateGenerator(ac_sType, pParams, sr,
                                                        bs, deterministic))
//  m_src(0)
{
    Q_ASSERT(m_pStrGen);
    // pParams->insert( std::pair<QString, QString>( "generator", "generator" )
    // );   //tell clients we have no inputs
}

WavGenerator::~WavGenerator()
{
}

void WavGenerator::Reset()
{
    m_pStrGen->Reset(); // must always be done
}

void WavGenerator::Prepare()
{
    m_pStrGen->Prepare();
}

bool WavGenerator::SetParameter(const QString &type, const int channel,
                                const QVariant &value)
// bool WavGenerator::SetParameter( const QString& ac_ParamID, const QString&
// ac_Val )
{
    Q_UNUSED(channel);
    /*
    if( !m_Params->HasParameter( ac_ParamID ) )
      return false;*/

    // resolve parameter type
    if (type == "gain") {
        const double dGain =
            ((data::WavFilterParameters *)m_Params)->baseGain() +
            (((data::WavFilterParameters *)m_Params)->invertGain()
                 ? -value.toDouble()
                 : +value.toDouble());

//#define PRINTWAVFILTER
#ifdef PRINTWAVFILTER
        qCDebug(APEX_RS, "new gain value: %f", dGain);
#endif

        if (channel != -1)
            qCDebug(APEX_RS,
                    "Warning: channel parameter not implemented in generator");

        m_pStrGen->mp_SetSignalAmp(dGain);

        return true;
    } else if (type == "frequency") {
        const double dFreq = value.toDouble();
#ifdef PRINTWAVFILTER
        qCDebug(APEX_RS, "new frequency: %f", dFreq);
#endif

        m_pStrGen->mp_SetFrequency(dFreq);
        return true;
    }
    return false;
}

StreamGenerator *WavGenerator::GetStreamGen() const
{
    Q_ASSERT(m_pStrGen);
    return m_pStrGen;
}
