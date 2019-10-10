/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "apextools/apextools.h"

#include "apextools/xml/xmltools.h"

#include "common/global.h"
#include "common/paths.h"

#include "resultparameters.h"

using namespace apex::data;
using namespace cmn;

ResultParameters::ResultParameters()
    : m_resultPage(QSL("resultsviewer.html")),
      m_showRealtime(false),
      m_showAfter(false)
{
}

ResultParameters::~ResultParameters()
{
}

bool ResultParameters::Parse(const QDomElement &p_paramElement)
{
    for (QDomElement currentNode = p_paramElement.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {

        QString tag = currentNode.tagName();
        QString value = currentNode.text();

        SetParameter(tag, value, currentNode);
    }

    return true;
}

bool ResultParameters::SetParameter(const QString &p_name,
                                    const QString &p_value,
                                    const QDomElement &p_elem)
{
    if (p_name == "matlabscript") {
        m_matlabScript = p_value;
    } else if (p_name == "subject") {
        m_subject = p_value;
    } else if (p_name == "resultparameters") {
        for (QDomElement current = p_elem.firstChildElement();
             !current.isNull(); current = current.nextSiblingElement()) {
            const QString tag = current.tagName();
            Q_ASSERT(tag == "parameter");
            QString name = current.attribute(QSL("name"));
            QString value = current.text();
            setResultParameter(name, value);
        }
    } else if (p_name == "resultscript") {
        m_extraScript = p_value;
    } else if (p_name == "showduringexperiment") {
        qCDebug(APEX_RS, "Set showrealtime to value: %s", qPrintable(p_value));
        m_showRealtime = ApexTools::bQStringToBoolean(p_value);
    } else if (p_name == "showafterexperiment") {
        m_showAfter = ApexTools::bQStringToBoolean(p_value);
    } else if (p_name == "page") {
        m_resultPage = p_value;
    } else {
        Q_ASSERT(0 && "Invalid tag");
        return false;
    }

    return true;
}

const QString &ResultParameters::matlabScript() const
{
    return m_matlabScript;
}

const QString &ResultParameters::subject() const
{
    return m_subject;
}

void ResultParameters::setSubject(const QString &subject)
{
    m_subject = subject;
}

bool ResultParameters::showResultsAfter() const
{
    return m_showAfter;
}

void ResultParameters::setShowResultsAfter(bool show)
{
    m_showAfter = show;
}

const QUrl &ResultParameters::resultPage() const
{
    return m_resultPage;
}

void ResultParameters::setResultPage(const QString &scriptname)
{
    m_resultPage = scriptname;
}

bool ResultParameters::showRTResults() const
{
    return m_showRealtime;
}

const QString ResultParameters::extraScript() const
{
    return m_extraScript;
}

void ResultParameters::setExtraScript(const QString &extraScript)
{
    m_extraScript = extraScript;
}

tScriptParameters ResultParameters::resultParameters() const
{
    return m_resultParameters;
}

void ResultParameters::setResultParameter(const QString &name,
                                          const QString &value)
{
    m_resultParameters.insert(name, value);
}

bool ResultParameters::operator==(const ResultParameters &other) const
{
    return m_matlabScript == other.m_matlabScript &&
           m_subject == other.m_subject &&
           m_extraScript == other.m_extraScript &&
           m_resultParameters == other.m_resultParameters &&
           m_resultPage == other.m_resultPage &&
           m_showRealtime == other.m_showRealtime &&
           m_showAfter == other.m_showAfter;
}
