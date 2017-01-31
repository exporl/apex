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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_MIXERFACTORY_H_
#define _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_MIXERFACTORY_H_

#include "apextools/xml/xercesinclude.h"

#include "factory.h"

namespace XERCES_CPP_NAMESPACE
{
  class DOMElement;
};
using namespace XERCES_CPP_NAMESPACE;

namespace apex
{
    namespace data {
        class MixerParameters;
    }


    namespace device {
        class IMixer;
    }


      /**
        * MixerFactory
        *   creates and deletes IMixers and MixerParameters.
        **************************************************** */
    class MixerFactory : public Factory<MixerFactory>
    {
      friend class appcore::Singleton<MixerFactory>;
    public:
        /**
          * Destructor.
          */
      ~MixerFactory();

        /**
          * Create a Mixer.
          * @param ac_sID a unique identifier
          * @param a_pParameters valid MixerParameters pointer
          * @return newly created IMixer or 0 for error
          */
      device::IMixer* mf_pCreateMixer( data::MixerParameters* a_pParameters );

        /**
          * Create MixerParameters.
          * @param a_pParamElement the DOMElement containing the parameter info
          * @return newly created parameters (always valid)
          */
      //MixerParameters* mf_pCreateMixerParameters( DOMElement* a_pParamElement );



    private:
        /**
          * Private constructor.
          */
      MixerFactory();
    };


}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_MIXERDEVICE_MIXERFACTORY_H_
