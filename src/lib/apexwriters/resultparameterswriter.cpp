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

#include "apexdata/result/resultparameters.h"

#include "apextools/apextools.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "resultparameterswriter.h"

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
    qCDebug(APEX_RS) << "Started writing results";

    //htmlpage
    //QString htmlPage = data.GetHtml();
    QString htmlPage = data.resultPage().toString();
    if (!htmlPage.isEmpty()) {
        result->appendChild(XMLutils::CreateTextElement(doc, "page",
                                                        htmlPage));
    }
    else {
        result->appendChild(XMLutils::CreateTextElement(doc, "page",
                            ""));
    }

    //resultparameters
    QMap<QString,QString> xp = data.resultParameters();
    if (!xp.isEmpty()) {
        DOMElement* xsp =doc->createElement(X("resultparameters")) ;
        result->appendChild(xsp) ;

        QMapIterator<QString,QString> it(xp);
        while (it.hasNext()) {
            it.next();
            qCDebug(APEX_RS) << "Resultparameters parsed:" << it.key() << " with value " << it.value();
            DOMElement* parameter = doc->createElement(X("parameter"));
            parameter->setAttribute( X("name"), S2X(it.key()));
            DOMNode* text = doc->createTextNode(S2X(it.value()));
            parameter->appendChild(text);
            xsp->appendChild(parameter);
         }
    }

    //showresultsduring
    if(data.showRTResults()){
        result->appendChild(XMLutils::CreateTextElement(doc, "showduringexperiment",
                             ApexTools::boolToString(true)));
    }

    //showresultsafter
    if (data.showResultsAfter()) {
        result->appendChild(XMLutils::CreateTextElement(doc, "showafterexperiment",
                            ApexTools::boolToString(true)));
    }

    //saveprocessedresults
    if (data.saveResults())
    {
        result->appendChild(XMLutils::CreateTextElement(doc,
                            "saveprocessedresults", ApexTools::boolToString(true)));
    }

    //matlabscript
    QString matlabScript = data.matlabScript();
    if (!matlabScript.isEmpty())
    {
        result->appendChild(XMLutils::CreateTextElement(doc, "matlabscript",
                            matlabScript));
    }

    //subject
    QString subject = data.subject();
    if (!subject.isEmpty())
    {
        result->appendChild(XMLutils::CreateTextElement(doc, "subject",
                            subject));
    }

    return result;
}


