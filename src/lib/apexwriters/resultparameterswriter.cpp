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

#include "resultparameterswriter.h"
#include "xml/apexxmltools.h"
#include "apextools.h"
#include "result/resultparameters.h"

#include "xml/xercesinclude.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

using apex::writer::ResultParametersWriter;
using apex::data::ResultParameters;


DOMElement* ResultParametersWriter::addElement(DOMDocument* doc,
        const ResultParameters& data)
{
    DOMElement* rootElement = doc->getDocumentElement();
    DOMElement* result = doc->createElement(X("results"));
    rootElement->appendChild(result);

    //xsltscript
    QString xsltScript = data.GetXsltScript();
    if (!xsltScript.isEmpty())
    {
        result->appendChild(XMLutils::CreateTextElement(doc, "xsltscript",
                            xsltScript));
    }

    QList<QPair<QString,QString> > xp = data.GetXsltParameters();
    if (!xp.isEmpty()) {
        DOMElement* xsp =doc->createElement(X("xsltscriptparameters")) ;
        result->appendChild(xsp) ;
        for (data::tXsltParameters::const_iterator it=xp.begin();
                it!=xp.end(); ++it) {
            DOMElement* parameter = doc->createElement(X("parameter"));
            parameter->setAttribute( X("name"), S2X(it->first));
            DOMNode* text = doc->createTextNode(S2X(it->second));
            parameter->appendChild(text);
            xsp->appendChild(parameter);
         }
    }

    //showresults
    if (data.showXsltResultsAfter())
    {
        result->appendChild(XMLutils::CreateTextElement(doc, "showresults",
                            ApexTools::boolToString(true)));
    }

    //saveprocessedresults
    if (data.GetSaveResults())
    {
        result->appendChild(XMLutils::CreateTextElement(doc,
                            "saveprocessedresults", ApexTools::boolToString(true)));
    }

    //matlabscript
    QString matlabScript = data.GetMatlabScript();
    if (!matlabScript.isEmpty())
    {
        result->appendChild(XMLutils::CreateTextElement(doc, "matlabscript",
                            matlabScript));
    }

    //subject
    QString subject = data.GetSubject();
    if (!subject.isEmpty())
    {
        result->appendChild(XMLutils::CreateTextElement(doc, "subject",
                            subject));
    }

    return result;
}


