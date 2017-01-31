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

#ifndef PARSERDEVICESPARSER_H
#define PARSERDEVICESPARSER_H

#include "apextypedefs.h"
#include "parser/apexparser.h"

//from libdata
#include "device/devicedata.h"     // we use a map of devicedata
#include "device/devicesdata.h"     // we use a map of devicedata

#include <QObject>

#include "xml/xercesinclude.h"
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
        //Q_OBJECT        // use tr
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
