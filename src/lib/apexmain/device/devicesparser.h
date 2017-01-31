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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_DEVICESPARSER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_DEVICE_DEVICESPARSER_H_

#include "apexdata/device/devicedata.h"
#include "apexdata/device/devicesdata.h"

#include "apextools/apextypedefs.h"

#include "apextools/xml/xercesinclude.h"

#include "parser/apexparser.h"

#include <QObject>

namespace XERCES_CPP_NAMESPACE
{
    class DOMElement;
};


namespace apex{

    namespace data {
        class PluginControllerData;
        class ParameterManagerData;
    }

    namespace parser {

        struct tAllDevices {
            data::DevicesData outputdevices;
            data::DevicesData controldevices;
        };

/**
        @author Tom Francart,,, <tom.francart@med.kuleuven.be>
*/
    class APEX_EXPORT DevicesParser: public Parser {
        Q_DECLARE_TR_FUNCTIONS ( DevicesParser );
public:
    DevicesParser();

    ~DevicesParser();

    //data::DevicesData Parse(XERCES_CPP_NAMESPACE::DOMElement* dom);
    tAllDevices Parse(XERCES_CPP_NAMESPACE::DOMElement* dom,data::ParameterManagerData* pm);
    data::DeviceData* ParseDevice( XERCES_CPP_NAMESPACE::DOMElement* p_base, data::ParameterManagerData* pm );


};

}
}

#endif
