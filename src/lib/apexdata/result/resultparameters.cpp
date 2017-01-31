/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "apextools/apextools.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "resultparameters.h"

using namespace apex::ApexXMLTools;
using namespace xercesc;
using namespace apex::data;

ResultParameters::ResultParameters(DOMElement* p_paramElement):
        m_resultPage("apex:resultsviewer.html"),
        m_showRealtime(false),
        m_showAfter(false),
        m_bSaveProcessedResults(false)
{
    Q_UNUSED(p_paramElement);

}

ResultParameters::ResultParameters():
        m_resultPage("apex:resultsviewer.html"),
        m_showRealtime(false),
        m_showAfter(false),
        m_bSaveProcessedResults(false)

{
}


ResultParameters::~ResultParameters()
{
}


bool ResultParameters::Parse(DOMElement* p_paramElement) {

        for (DOMNode* currentNode=p_paramElement->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling()) {
                Q_ASSERT(currentNode);
                if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
//                      DOMElement* el = (DOMElement*) currentNode;

                        QString tag   = XMLutils::GetTagName( currentNode );
                        QString id    = XMLutils::GetAttribute( currentNode, "id" );
                        QString value = XMLutils::GetFirstChildText( currentNode );

                        SetParameter(tag, id, value, (DOMElement*) currentNode);
                        //insert( tParamMapPair(tag,value) );           // insert in map [ stijn ] removed this, quite redundant

                } else {
                        Q_ASSERT(0);            // TODO
                }
        }

        return true;
}


bool ResultParameters::SetParameter(const QString& p_name, const QString& /*p_id*/, const QString& p_value, DOMElement* p_elem)
{
    if (p_name=="matlabscript") {
        m_matlabScript = p_value;
    } else if (p_name=="subject") {
        m_subject=p_value;
    } else if (p_name == "saveprocessedresults") {
        m_bSaveProcessedResults = ApexTools::bQStringToBoolean( p_value );
    } else if(p_name == "resultparameters") {
        for (DOMNode* current=p_elem->getFirstChild(); current!=0;
             current=current->getNextSibling()) {
            Q_ASSERT(current);
            Q_ASSERT(current->getNodeType() == DOMNode::ELEMENT_NODE);
            const QString tag = XMLutils::GetTagName( current);
            Q_ASSERT(tag=="parameter");

            QString name = XMLutils::GetAttribute( current, "name" );
            QString value = XMLutils::GetFirstChildText(current);

            setResultParameter(name,value);
        }
    } else if (p_name == "resultscript") {
        m_extraScript = p_value;
    } else if(p_name == "showduringexperiment") {
        qCDebug(APEX_RS, "Set showrealtime to value: %s", qPrintable (p_value));
        m_showRealtime = ApexTools::bQStringToBoolean( p_value );
    } else if(p_name == "showafterexperiment") {
        m_showAfter =  ApexTools::bQStringToBoolean( p_value );
    } else if(p_name == "page") {
            m_resultPage = p_value;
    } else {
        Q_ASSERT(0 && "Invalid tag");
                return false;
        }

        return true;
}

const QString& ResultParameters::matlabScript() const { return m_matlabScript; }
const QString& ResultParameters::subject() const { return m_subject; }
void ResultParameters::setSubject(const QString &subject){m_subject = subject;}

bool ResultParameters::showResultsAfter() const { return m_showAfter; }
void ResultParameters::setShowResultsAfter(bool show) {m_showAfter = show;}

bool ResultParameters::saveResults() const { return m_bSaveProcessedResults; }
void ResultParameters::setSaveResults(bool save) {m_bSaveProcessedResults=save; }

const QUrl& ResultParameters::resultPage() const { return m_resultPage; }
void ResultParameters::setResultPage(const QString &scriptname) { m_resultPage=scriptname; }
bool ResultParameters::showRTResults() const { return m_showRealtime;}

const QString ResultParameters::extraScript() const {return m_extraScript; }

tScriptParameters ResultParameters::resultParameters() const { return mResultParameters; }


bool ResultParameters::operator==(const ResultParameters& other) const
{
    return  /*ApexParameters::operator==(other) &&*/
            m_matlabScript == other.m_matlabScript &&
            m_subject == other.m_subject &&
            m_bSaveProcessedResults == other.m_bSaveProcessedResults &&
            m_resultPage == other.m_resultPage &&
            m_showRealtime == other.m_showRealtime &&
            m_showAfter == other.m_showAfter;
}


void ResultParameters::setResultParameter(const QString& name, const QString& value)
{
    //mXsltParameters[name]=value;
    mResultParameters.insert(name, value);

}

