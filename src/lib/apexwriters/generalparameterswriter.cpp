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

#include "generalparameterswriter.h"
#include "xml/apexxmltools.h"
#include "apextools.h"
#include "parameters/generalparameters.h"

#include "xml/xercesinclude.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

using apex::writer::GeneralParametersWriter;
using apex::data::GeneralParameters;


DOMElement* GeneralParametersWriter::addElement(DOMDocument* doc,
        const GeneralParameters& data)
{
    DOMElement* rootElement = doc->getDocumentElement();
    DOMElement* general = doc->createElement(X("general"));
    rootElement->appendChild(general);

    //exitafter
    if (data.GetExitAfter())
    {
        general->appendChild(XMLutils::CreateTextElement(doc, "exitafter",
                             ApexTools::boolToString(true)));
    }

    //autosave
    if (data.GetAutoSave())
    {
        general->appendChild(XMLutils::CreateTextElement(doc, "autosave",
                             ApexTools::boolToString(true)));
    }

    //waitforstart
    if (data.GetWaitForStart())
    {
        general->appendChild(XMLutils::CreateTextElement(doc, "waitforstart",
                             ApexTools::boolToString(true)));
    }

    //allowskip
    if (data.GetAllowSkip())
    {
        general->appendChild(XMLutils::CreateTextElement(doc, "allowskip",
                             ApexTools::boolToString(true)));
    }

    //runoutputtest
    if (data.RunOutputTest())
    {
        general->appendChild(XMLutils::CreateTextElement(doc, "runoutputtest",
                             ApexTools::boolToString(true)));
    }

    //outputtestinput
    QString input = data.OutputTestInput();
    if (!input.isEmpty())
    {
        general->appendChild(XMLutils::CreateTextElement(doc, "outputtestinput",
                             input));
    }

    //scriptlibrary
    QString lib = data.GetScriptLibrary();
    if (!lib.isEmpty())
    {
        general->appendChild(XMLutils::CreateTextElement(doc, "scriptlibrary",
                             lib));
    }

    return general;
}























