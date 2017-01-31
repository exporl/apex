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

#include "iapexdevice.h"

#include "parameters/parametermanager.h"

namespace apex
{
namespace device
{

IApexDevice::IApexDevice(data::DeviceData* a_pParameters) :
                                            mc_sID2( a_pParameters->id() ),
                                            m_pParameters( a_pParameters ),
                                            mv_bNeedsRestore( false )
{
    Q_ASSERT( a_pParameters );
}

IApexDevice::~IApexDevice()
{
}

bool IApexDevice::SetParameter(const data::Parameter& name, const QVariant& value)
{
    if (name.owner() != GetID())
        return false;

    SetParameter(name.type(), name.channel(), value);
    return true;
}

const QString& IApexDevice::GetID() const
{
    return mc_sID2;
}

bool IApexDevice::HasParameter(const QString& type) const
{
    return m_pParameters->hasParameter(type);
}

void IApexDevice::SetParameters(const apex::ParameterManager& pm)
{
    data::ParameterValueMap params = pm.parametersForOwner(GetID());

    data::ParameterValueMap::const_iterator it;
    for (it = params.begin(); it != params.end(); ++it)
        SetParameter ( it.key().type(), it.key().channel(), it.value() );
}

void IApexDevice::RestoreParameters()
{
    Q_FOREACH (data::Parameter param, m_pParameters->parameters())
        SetParameter(param.type(), param.channel(), param.defaultValue());
}

const apex::data::DeviceData& IApexDevice::GetParameters() const
{
    return *m_pParameters;
}

data::DeviceData* IApexDevice::ModParameters() const
{
    return m_pParameters;
}

QString IApexDevice::GetResultXML() const
{
    return QString();
}

QString IApexDevice::GetEndXML() const
{
    return QString();
}

bool IApexDevice::GetInfo(const unsigned, void*) const
{
    return false;
}


}//ns device
}//ns apex

