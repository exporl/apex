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

#include "apexdata/result/resultparameters.h"

#include "apextools/apextools.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"

#include "resultparameterswriter.h"

using apex::writer::ResultParametersWriter;
using apex::data::ResultParameters;

QDomElement ResultParametersWriter::addElement(QDomDocument *doc,
                                               const ResultParameters &data)
{
    QDomElement rootElement = doc->documentElement();
    QDomElement result = doc->createElement(QSL("results"));
    rootElement.appendChild(result);
    qCDebug(APEX_RS) << "Started writing results";

    // htmlpage
    // QString htmlPage = data.GetHtml();
    QString htmlPage = data.resultPage().toString();
    if (!htmlPage.isEmpty()) {
        result.appendChild(XmlUtils::createTextElement(doc, "page", htmlPage));
    } else {
        result.appendChild(XmlUtils::createTextElement(doc, "page", ""));
    }

    // resultparameters
    QMap<QString, QString> xp = data.resultParameters();
    if (!xp.isEmpty()) {
        QDomElement xsp = doc->createElement(QSL("resultparameters"));
        result.appendChild(xsp);

        QMapIterator<QString, QString> it(xp);
        while (it.hasNext()) {
            it.next();
            qCDebug(APEX_RS) << "Resultparameters parsed:" << it.key()
                             << " with value " << it.value();
            QDomElement parameter = doc->createElement(QSL("parameter"));
            parameter.setAttribute(QSL("name"), it.key());
            QDomNode text = doc->createTextNode(it.value());
            parameter.appendChild(text);
            xsp.appendChild(parameter);
        }
    }

    // resultscript
    if (!data.extraScript().isEmpty()) {
        result.appendChild(XmlUtils::createTextElement(doc, "resultscript",
                                                       data.extraScript()));
    }

    // showresultsduring
    if (data.showRTResults()) {
        result.appendChild(XmlUtils::createTextElement(
            doc, "showduringexperiment", ApexTools::boolToString(true)));
    }

    // showresultsafter
    if (data.showResultsAfter()) {
        result.appendChild(XmlUtils::createTextElement(
            doc, "showafterexperiment", ApexTools::boolToString(true)));
    }

    // saveprocessedresults
    if (data.saveResults()) {
        result.appendChild(XmlUtils::createTextElement(
            doc, "saveprocessedresults", ApexTools::boolToString(true)));
    }

    // matlabscript
    QString matlabScript = data.matlabScript();
    if (!matlabScript.isEmpty()) {
        result.appendChild(
            XmlUtils::createTextElement(doc, "matlabscript", matlabScript));
    }

    // subject
    QString subject = data.subject();
    if (!subject.isEmpty()) {
        result.appendChild(
            XmlUtils::createTextElement(doc, "subject", subject));
    }

    return result;
}
