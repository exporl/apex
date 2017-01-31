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

#ifndef _EXPORL_SRC_LIB_APEXWRITERS_CORRECTORWRITER_H_
#define _EXPORL_SRC_LIB_APEXWRITERS_CORRECTORWRITER_H_

#include "apextools/global.h"

#include "apextools/xml/xercesinclude.h"

namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
}

namespace apex
{
namespace data
{
class CorrectorData;
}

namespace writer
{

/**
*@author Job Noorman <jobnoorman@gmail.com>
*/
class APEXWRITERS_EXPORT CorrectorWriter
{
    public:
        /**
        * Adds a <corrector> element to the document.
        *
        * @param elem  the element to place the element in
        * @param data the data to create the element with
        *
        * @return the created element
        */
        static XERCES_CPP_NAMESPACE::DOMElement* addElement(XERCES_CPP_NAMESPACE::DOMElement* elem, const data::CorrectorData& data);

};

}

}

#endif
