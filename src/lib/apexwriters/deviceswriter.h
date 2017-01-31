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

#ifndef DEVICESWRITER_H
#define DEVICESWRITER_H

/**
 *@author Job Noorman
 */

//#include "device/devicedata.h"

#include "global.h"

#include "xml/xercesinclude.h"
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
using namespace XERCES_CPP_NAMESPACE;

class APEXWRITERS_EXPORT DevicesWriter
{
    public:
        /**
        *Adds a <device> element to the document
        *
        *@param doc the document to place the element in
        *@param data the data to create the element with
        */
        static DOMElement* addElement ( DOMDocument* doc, const data::DevicesData& data );
    private:
        /**
         * Finishes a partly created <device> element as a wavdevice
         *
         * @param dev the <device> element that needs to be finished
         * @param data the data to finish this element with
         */
        static void finishAsWav ( DOMElement* dev, const data::WavDeviceData& data );
        /**
         * Finishes a partly created <device> element as an l34device
         *
         * @param dev the <device> element that needs to be finished
         * @param data the data to finish this element with
         */
        static void finishAsL34 ( DOMElement* dev, const data::L34DeviceData& data );
};
}
}

#endif
