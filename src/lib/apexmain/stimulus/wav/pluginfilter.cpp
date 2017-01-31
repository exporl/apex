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
 
#include "filter/filterdata.h"
#include "filter/pluginfilterdata.h"

#include "parameters/simpleparameters.h"

#include "exceptions.h"
#include "pluginfilter.h"

#include <QPluginLoader>
#include <QString>
#include <QDebug>

namespace apex
{

namespace stimulus
{

PluginFilter::PluginFilter (const QString &name,
        data::PluginFilterData *parameters,
                            unsigned long sr, unsigned bs) :
    WavFilter (name, parameters, sr, bs),
    m_data (parameters),
    streamLength (0)
{
    // Tom says: this is destroyed somewhere, but he does not know where
    // ...or maybe not FIXME
    //
    m_StrProc = new PluginFilterProcessor (m_data->numberOfChannels(), GetBlockSize(), GetSampleRate(),  m_data->plugin());
    // FIXME error handling
}

PluginFilter::~PluginFilter()
{
}

bool PluginFilter::SetParameter( const QString& type, const int channel, const QVariant& value )
{
    try {
        return m_StrProc->setParameter ( type, channel,value.toString());
    } catch (...) {
        return false;
    }
}

void PluginFilter::Reset()
{
    m_StrProc->resetParameters();
}

void PluginFilter::mp_SetStreamLength (const unsigned long ac_nSamples)
{
    streamLength = ac_nSamples;
}

void PluginFilter::Prepare()
{
    m_StrProc->Prepare (streamLength);
}

streamapp::IStreamProcessor *PluginFilter::GetStrProc() const
{
    return m_StrProc;
}

} // namespace stimulus

} // namespace apex
