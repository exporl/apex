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

#include "apexdata/mixerdevice/mixerparameters.h"

#include "apextools/exceptions.h"

#include "imixer.h"

using namespace apex;
using namespace apex::device;

IMixer::IMixer( data::MixerParameters* a_pParameters ) :
ControlDevice(  a_pParameters )
{
    if( a_pParameters->hasParameter( "gain" ) )
        throw ApexStringException( "IMixer: cannot have a parameter named \"gain\" " );
}

IMixer::~IMixer()
{
}

bool IMixer::SetParameter ( const QString& type, const int channel, const QVariant& value )
{


    if (type=="gain") {
        const double dGain = mf_dGetGainInRange( value.toDouble() );

        if (channel==-1)
            mp_SetGain( dGain );
        else
            mp_SetGain( dGain, channel );

        return true;
    } else {
        return false;
    }

}

namespace
{
    const QString sc_sErrorMess( "IMixer: Gain out of range, coercing to " );
}

double IMixer::mf_dGetGainInRange( const double ac_dGain )
{
    mv_sInfo = QString();

    const double dMax = mf_dGetMaxGain();
    if( ac_dGain > dMax )
    {
        mv_sInfo = sc_sErrorMess + QString::number( dMax );
        return dMax;
    }

    const double dMin = mf_dGetMinGain();
    if( ac_dGain < dMin )
    {
        mv_sInfo = sc_sErrorMess + QString::number( dMin );
        return dMin;
    }

    return ac_dGain;
}

bool IMixer::GetInfo( const unsigned ac_nType, void* a_pInfo ) const
{
    if( mv_sInfo.isEmpty() )
        return false;

    if( ac_nType == IApexDevice::mc_eOutOfRange )
    {
        *( (QString*)a_pInfo )= mv_sInfo;
        return true;
    }

    return false;
}

void IMixer::Reset() {
    mp_SetGain(0);
}

void IMixer::Prepare() {
    qCDebug(APEX_RS, "IMixer::Prepare: fixme: setting already done on setparameter");
}
