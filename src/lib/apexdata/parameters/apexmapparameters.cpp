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

#include "apexmapparameters.h"
#include "xml/xmlkeys.h"
#include "xml/apexxmltools.h"
#include <cassert>

#include "xml/xercesinclude.h"
using namespace xercesc;

using namespace apex;
using namespace apex::data;
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;

tParamMapCIt ApexMapParameters::begin() const
{
    return parameters.begin();
}

tParamMapIt ApexMapParameters::begin()
{
    return parameters.begin();
}

tParamMapCIt ApexMapParameters::end() const
{
    return parameters.end();
}

void ApexMapParameters::insert(QString name, QString value)
{
    parameters.insert(name, value);
}

void ApexMapParameters::insert(tParamMapPair pair)
{
    insert(pair.first, pair.second);
}

void ApexMapParameters::remove(const QString& name)
{
    parameters.remove(name);
}

void ApexMapParameters::erase(tParamMapIt it)
{
    parameters.erase(it);
}

bool ApexMapParameters::isEmpty() const
{
	return parameters.isEmpty();
}

ApexMapParameters::ApexMapParameters( DOMElement* a_pParamElement )
{
  if( a_pParamElement )
    Parse( a_pParamElement );
}

ApexMapParameters::~ApexMapParameters()
{
}

bool ApexMapParameters::Parse( DOMElement* p_paramElement )
{
  if( !p_paramElement )
    return false;

  for (DOMNode* currentNode=p_paramElement->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling()) {
    assert(currentNode);
    assert(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

    DOMElement* el = (DOMElement*) currentNode;

    const QString type  = XMLutils::GetAttribute( el, gc_sType );
    const QString id    = XMLutils::GetAttribute( el, gc_sID );
    const QString value = XMLutils::GetFirstChildText( el );
    QString tag         = XMLutils::GetTagName( el );

  if (type.isEmpty() && id.isEmpty()) {
    // don't add parameter to map, it cannot be referenced anyway
      qDebug("Cannot add parameter to map");
      continue;
  }

      //[ stijn ] if there are 2 parameters with the same type
      //the first one will be overwritten
      //this cannot be allowed
/*  qDebug("type=" + type);
  assert( find( type ) == end() && "sorry, no duplicate types in parameters!" );*/
  // [Tom] deleted

    if( type.isEmpty() )
    {
      assert( !id.isEmpty() );
      SetParameter( id, value );
    }
    else
      SetParameter( id, value );
  }
  return true;
}

void ApexMapParameters::SetParameter( const QString& ac_sName, const QString& ac_sValue )
{
  parameters[ac_sName] = ac_sValue;
}


QString ApexMapParameters::GetParameter( const QString& p_name ) const
{
    return parameters.value(p_name);
}

bool ApexMapParameters::HasParameter( const QString& p_name ) const
{
    return parameters.contains(p_name);
}

QString ApexMapParameters::mf_sPrint( const QString& /*ac_Seperator*/ ) const
{
  QString sRet;

//   tParamMapCIt itB = tParamMap::begin();
//   tParamMapCIt itE = tParamMap::end();
//   for( ; itB != itE ; ++itB )
//     sRet.append( (*itB).first + " = " + (*itB).second + ac_Seperator );
//   sRet.resize( sRet.length() - ac_Seperator.length() );     //strip last seperator added

  return sRet;
}

bool ApexMapParameters::operator==(const ApexMapParameters& other) const
{
    return parameters == other.parameters;
}
