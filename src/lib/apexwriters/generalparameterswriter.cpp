/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/parameters/generalparameters.h"

#include "apextools/apextools.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "generalparameterswriter.h"

using apex::writer::GeneralParametersWriter;
using apex::data::GeneralParameters;

QDomElement GeneralParametersWriter::addElement(QDomDocument *doc,
                                                const GeneralParameters &data)
{
    QDomElement rootElement = doc->documentElement();
    QDomElement general = doc->createElement(QSL("general"));
    rootElement.appendChild(general);

    // exitafter
    if (data.GetExitAfter()) {
        general.appendChild(XmlUtils::createTextElement(
            doc, "exitafter", ApexTools::boolToString(true)));
    }

    // autosave
    if (data.GetAutoSave()) {
        general.appendChild(XmlUtils::createTextElement(
            doc, "autosave", ApexTools::boolToString(true)));
    }

    // waitforstart
    if (data.GetWaitForStart()) {
        general.appendChild(XmlUtils::createTextElement(
            doc, "waitforstart", ApexTools::boolToString(true)));
    }

    // allowskip
    if (data.GetAllowSkip()) {
        general.appendChild(XmlUtils::createTextElement(
            doc, "allowskip", ApexTools::boolToString(true)));
    }

    // runoutputtest
    if (data.RunOutputTest()) {
        general.appendChild(XmlUtils::createTextElement(
            doc, "runoutputtest", ApexTools::boolToString(true)));
    }

    // outputtestinput
    QString input = data.OutputTestInput();
    if (!input.isEmpty()) {
        general.appendChild(
            XmlUtils::createTextElement(doc, "outputtestinput", input));
    }

    // scriptlibrary
    QString lib = data.GetScriptLibrary();
    if (!lib.isEmpty()) {
        general.appendChild(
            XmlUtils::createTextElement(doc, "scriptlibrary", lib));
    }

    return general;
}
