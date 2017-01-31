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

#ifdef SETMIXER

#include "apexdata/mixerdevice/mixerparameters.h"

#include "apextools/exceptions.h"

#include "apextools/xml/xercesinclude.h"

#include "pa5device/pa5device.h"

#include "imixer.h"
#include "mixerfactory.h"
#include "streamappmixer.h"

using namespace apex;
using namespace apex::device;
using namespace apex::data;
using namespace xercesc;

MixerFactory::MixerFactory()
{
}

MixerFactory::~MixerFactory()
{
}

apex::device::IMixer* MixerFactory::mf_pCreateMixer(data::MixerParameters* a_pParameters)
{


    mp_ClearErrors();
    try
    {
        device::IMixer* pRet = 0;
        const MixerType eType = a_pParameters->type();
        if (eType == DEFAULT_MIXER || eType == RME_MIXER)
            pRet = new StreamAppMixer(a_pParameters);
#ifdef PA5
        else if (eType == PA5_MIXER)
            pRet = new Pa5Device(a_pParameters);
#endif
        if (pRet)
        {
            Q_FOREACH (data::Parameter param, a_pParameters->parameters())
            {
                if (param.type() == "gain")
                {
                    int channel = param.channel();
                    if (channel != -1 && channel >= (int)(pRet->mf_nGetNumChannels()))
                    {
                        delete pRet;
                        throw(ApexStringException("MixerParameters: bad channel number (" + QString::number(channel) + ") for " + a_pParameters->id() + "; Note: channels are zero based"));
                    }
                }
            }
        }
        return pRet;
    }
    catch (ApexStringException& e)
    {
        m_Error->addError("MixerFactory::mf_pCreateMixer", e.what());
        return 0;
    }
}

/*MixerParameters* MixerFactory::mf_pCreateMixerParameters( DOMElement* a_pParamElement )
{
  mp_ClearErrors();
  return (MixerParameters*) keep( new MixerParameters( a_pParamElement ) );
}*/



#endif //SETMIXER

