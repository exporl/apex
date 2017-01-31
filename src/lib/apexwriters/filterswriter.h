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

#ifndef FILTERSWRITER_H
#define FILTERSWRITER_H

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
class FiltersData;
class FilterData;
}

namespace writer
{
using namespace XERCES_CPP_NAMESPACE;

/**
*@author Job Noorman <jobnoorman@gmail.com>
*/
class APEXWRITERS_EXPORT FiltersWriter
{
    public:
        /**
        *Adds a <filters> element to the document.
        *
        *@param doc  the document to place the element in
        *@param data the data to create the element with
        *
        *@return the created element
         */
        static DOMElement* addElement(DOMDocument* doc, const data::FiltersData& data);

    private:
        /**
        *Fills the given element with the given data.
        *
        *@param data  the data to fill the element with
        *@param theElement the element to fill
        */
        static void fillFilterElement(const data::FilterData& data, DOMElement* theElement);
};

}

}

#endif
