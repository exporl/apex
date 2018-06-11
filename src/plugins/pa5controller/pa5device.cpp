/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "pa5device.h"

#ifdef PA5

#include "exceptions.h"
#include "pa5_actx.h"

using namespace apex;
using namespace apex::device;

Pa5Device::Pa5Device(data::MixerParameters *a_pParameters)
    : IMixer(a_pParameters), m_pActXObj(new Pa5ActX()), m_nNumPA5s(0)
{
    if (!m_pActXObj->Connect(4, 1))
        throw(ApexStringException("Pa5Device: couldn't connect to PA5"));

    m_nNumPA5s = 1; // TODO multiple devices
}

Pa5Device::~Pa5Device()
{
    delete m_pActXObj;
}

double Pa5Device::mf_dGetMaxGain() const
{
    return 0.0;
}

double Pa5Device::mf_dGetMinGain() const
{
    return -120.0;
}

void Pa5Device::mp_SetGain(const double ac_dGainIndB)
{
    m_pActXObj->SetAtten(-ac_dGainIndB);
}

void Pa5Device::mp_SetGain(const double ac_dGainIndB,
                           const unsigned ac_nChannel)
{
    m_pActXObj->SetAtten(-ac_dGainIndB);
}

double Pa5Device::mf_dGetGain(const unsigned ac_nChannel) const
{
    return (0.0 - m_pActXObj->GetAtten());
}

#endif //#ifdef PA5
