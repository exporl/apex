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

#include "apexdata/corrector/correctordata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "correctorwriter.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

using apex::writer::CorrectorWriter;
using apex::data::CorrectorData;

DOMElement* CorrectorWriter::addElement(DOMElement* elem,
                                        const CorrectorData& data)
{
    DOMDocument* doc = elem->getOwnerDocument();
    DOMElement* corrector = doc->createElement(X("corrector"));
    elem->appendChild(corrector);

    QString type;
    switch (data.type())
    {
        case CorrectorData::EQUAL:
        {
            type = "apex:isequal";
            break;
        }
        default:
        {
             qFatal("Invalid corrector type");
        }
    }

    corrector->setAttribute(X("xsi:type"), S2X(type));
    return corrector;
}































