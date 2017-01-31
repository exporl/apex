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

#include "resultparameters.h"
#include "apextools.h"
#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;

#include "xml/xercesinclude.h"
using namespace xercesc;
using namespace apex::data;

ResultParameters::ResultParameters(DOMElement* p_paramElement):
        ApexParameters(p_paramElement),
        m_showRealtime(false),
        m_showAfter(false),
        m_bShowResultsXslt(false),
        m_bSaveProcessedResults(false)
{
    
}

ResultParameters::ResultParameters():
        ApexParameters(0),
        m_showRealtime(false),
        m_showAfter(false),
        m_bShowResultsXslt(false),
        m_bSaveProcessedResults(false)
{
}


ResultParameters::~ResultParameters()
{
}


bool ResultParameters::SetParameter(const QString& p_name, const QString& /*p_id*/, const QString& p_value, DOMElement* p_elem)
{
    if (p_name=="xsltscript") {
        //qDebug("Set xsltscript to value: %s", qPrintable (p_value));
        m_xsltScript = p_value;
    } else if (p_name=="xsltscriptparameters") {
        for (DOMNode* current=p_elem->getFirstChild(); current!=0;
                current=current->getNextSibling()) {
            Q_ASSERT(current);
            Q_ASSERT(current->getNodeType() == DOMNode::ELEMENT_NODE);
            const QString tag = XMLutils::GetTagName( current);
            Q_ASSERT(tag=="parameter");

            QString name = XMLutils::GetAttribute( current, "name" );
            QString value = XMLutils::GetFirstChildText(current);

            //mXsltParameters[name]=value;
            //mXsltParameters.push_back( QPair<QString,QString>(name, value));
            setXsltParameter(name,value);
        }
    } else if (p_name=="matlabscript") {
        //qDebug("Set matlabscript to value: %s", qPrintable (p_value));
        m_matlabScript = p_value;
    } else if (p_name=="subject") {
        m_subject=p_value;
    } else if (p_name == "saveprocessedresults") {
        m_bSaveProcessedResults = ApexTools::bQStringToBoolean( p_value );
    } else if (p_name == "showresults") {
        m_bShowResultsXslt = ApexTools::bQStringToBoolean( p_value );
    } else if (p_name == "javascript") {
        for (DOMNode* current=p_elem->getFirstChild(); current!=0;
        current=current->getNextSibling()) {
            Q_ASSERT(current);
            Q_ASSERT(current->getNodeType() == DOMNode::ELEMENT_NODE);
            const QString tag( XMLutils::GetTagName(current) );
            const QString value( XMLutils::GetFirstChildText(current));

            if (tag=="page") {
                m_resultPage = value;
                m_showRealtime = true;
            } else if (tag=="realtime")
                m_showRealtime = ApexTools::bQStringToBoolean( value );
            else if (tag=="showafterexperiment")
                m_showAfter =  ApexTools::bQStringToBoolean( value );
            else
                qFatal("Invalid tag");
        }
        
        
    } else {
        Q_ASSERT(0 && "Invalid tag");
                return false;
        }

        return true;
}

bool ResultParameters::operator==(const ResultParameters& other) const
{
    return  ApexParameters::operator==(other) &&
            m_xsltScript == other.m_xsltScript &&
            m_matlabScript == other.m_matlabScript &&
            m_subject == other.m_subject &&
            m_bShowResultsXslt == other.m_bShowResultsXslt &&
            m_bSaveProcessedResults == other.m_bSaveProcessedResults &&
            m_resultPage == other.m_resultPage &&
            m_showRealtime == other.m_showRealtime &&
            m_showAfter == other.m_showAfter;
}


void ResultParameters::setXsltParameter(const QString& name, const QString& value)
{
    //mXsltParameters[name]=value;
    mXsltParameters.push_back( QPair<QString,QString>(name, value));
}

