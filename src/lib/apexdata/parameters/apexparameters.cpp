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

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "apexparameters.h"

#include <QMap>
#include <QString>

#ifdef CLEBS_DEBUG
#include <iostream>
#endif

using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using namespace apex::data;
using namespace xercesc;

//namespace apex {

ApexParameters::const_iterator::const_iterator(
        const QMap<QString,QString>::const_iterator& it)
{
    *this = it;
}

ApexParameters::const_iterator& ApexParameters::const_iterator::
        operator=(const QMap<QString,QString>::const_iterator& it)
{
    this->it = it;
    return *this;
}
ApexParameters::const_iterator&
        ApexParameters::const_iterator::operator++() //prefix++
{
    it++;
    return *this;
}

ApexParameters::const_iterator
        ApexParameters::const_iterator::operator++(int) //postfix++
{
    const_iterator copy(it);
    ++(*this);
    return copy;
}

bool ApexParameters::const_iterator::operator==(const const_iterator& other) const
{
    return it == other.it;
}

bool ApexParameters::const_iterator::operator!=(const const_iterator& other) const
{
    return !operator==(other);
}

const QString& ApexParameters::const_iterator::key()
{
    return it.key();
}

const QString& ApexParameters::const_iterator::value()
{
    return it.value();
}

QMap<QString,QString>::const_iterator ApexParameters::begin() const
{
    return parameters.begin();
}

QMap<QString,QString>::const_iterator ApexParameters::end() const
{
    return parameters.end();
}


ApexParameters::ApexParameters(DOMElement* p_paramElement): m_paramElement(p_paramElement)
{
        //Q_ASSERT(p_paramElement);
}

bool ApexParameters::Parse(DOMElement* p_paramElement) {
        if (p_paramElement!=0)
                m_paramElement=p_paramElement;

        if (m_paramElement==0) {
                qCDebug(APEX_RS, "ApexParameters::Parse: m_paramElement == NULL");
                return true;
        }

        for (DOMNode* currentNode=m_paramElement->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling()) {
                Q_ASSERT(currentNode);
                if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE) {
//                      DOMElement* el = (DOMElement*) currentNode;

                        QString tag   = XMLutils::GetTagName( currentNode );
                        QString id    = XMLutils::GetAttribute( currentNode, gc_sID );
                        QString value = XMLutils::GetFirstChildText( currentNode );

                        SetParameter(tag, id, value, (DOMElement*) currentNode);
                        //insert( tParamMapPair(tag,value) );           // insert in map [ stijn ] removed this, quite redundant

                } else {
                        Q_ASSERT(0);            // TODO
                }
        }

        return true;
}


ApexParameters::~ApexParameters()
{

}


/*bool ApexParameters::SetParameter(QString& p_name, QString& p_id, QString& p_value) {
        return true;
}*/

QString ApexParameters::GetParameter( const QString& p_name ) const
{
    return parameters.value(p_name);
}

bool ApexParameters::HasParameter (const QString& p_name) const
{
    return parameters.contains(p_name);
}

/**
 * Add errors/warnings to errorHandler
 * @param errorHandler
 * @return true if no fatal errors
 */
bool ApexParameters::CheckParameters()
{
    return true;
}

bool ApexParameters::operator==(const ApexParameters& other) const
{
    return parameters == other.parameters;
}

//}
