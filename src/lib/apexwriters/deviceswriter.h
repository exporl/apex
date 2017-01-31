/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _EXPORL_SRC_LIB_APEXWRITERS_DEVICESWRITER_H_
#define _EXPORL_SRC_LIB_APEXWRITERS_DEVICESWRITER_H_

#include "apextools/global.h"

#include "apextools/xml/xercesinclude.h"

namespace XERCES_CPP_NAMESPACE
{
class DOMDocument;
class DOMElement;
}

namespace apex
{
namespace data
{
class DevicesData;
class WavDeviceData;
class L34DeviceData;
}

namespace writer
{

class APEXWRITERS_EXPORT DevicesWriter
{
    public:
        /**
        *Adds a <device> element to the document
        *
        *@param doc the document to place the element in
        *@param data the data to create the element with
        */
        static XERCES_CPP_NAMESPACE::DOMElement* addElement (XERCES_CPP_NAMESPACE::DOMDocument* doc, const data::DevicesData& data );
    private:
        /**
         * Finishes a partly created <device> element as a wavdevice
         *
         * @param dev the <device> element that needs to be finished
         * @param data the data to finish this element with
         */
        static void finishAsWav (XERCES_CPP_NAMESPACE::DOMElement* dev, const data::WavDeviceData& data );
        /**
         * Finishes a partly created <device> element as an l34device
         *
         * @param dev the <device> element that needs to be finished
         * @param data the data to finish this element with
         */
        static void finishAsL34 (XERCES_CPP_NAMESPACE::DOMElement* dev, const data::L34DeviceData& data );
};
}
}

#endif
