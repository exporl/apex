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

#include "generalparameters.h"
#include "apextools.h"
#include "xml/xercesinclude.h"
#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;

using namespace xercesc;
using namespace apex::data;

GeneralParameters::GeneralParameters(DOMElement* p_paramElement)
 : ApexParameters(p_paramElement),
    m_bExitAfter(false),
    m_bAutoSave(false),
    m_bWaitForStart( false ),
    m_bAllowSkip( true ),
    m_bRunOutputTest( false )
{
}

GeneralParameters::~GeneralParameters()
{
}

bool GeneralParameters::SetParameter(const QString& p_name, const QString& /*p_id*/, const QString& p_value, DOMElement* p_element)
{
  if (p_name == "exitafter") {
    m_bExitAfter = ApexTools::bQStringToBoolean( p_value );
  } else if (p_name == "autosave") {
    m_bAutoSave = ApexTools::bQStringToBoolean( p_value );
  } else if (p_name == "waitforstart") {
    m_bWaitForStart = ApexTools::bQStringToBoolean( p_value );
  } else if (p_name == "allowskip") {
    m_bAllowSkip = ApexTools::bQStringToBoolean( p_value );
  } else if (p_name == "runoutputtest") {
    m_bRunOutputTest = ApexTools::bQStringToBoolean( p_value );
  } else if (p_name == "outputtestinput") {
    m_sOutputTestInput = p_value;
  } else if (p_name =="scriptlibrary") {
      m_scriptLibrary = p_value;
  } else if (p_name =="scriptparameters") {

      for ( DOMNode* it = p_element->getFirstChild() ; it != 0 ;
              it = it->getNextSibling() )
      {
          if ( it->getNodeType() != DOMNode::ELEMENT_NODE )
              continue;

          DOMElement* currentNode = (DOMElement*) it;

          const QString tag( XMLutils::GetTagName( it ) );

          if (tag=="parameter")
          {
              QString name = XMLutils::GetAttribute( currentNode, "name" );
              QVariant value = XMLutils::GetFirstChildText(currentNode);
              m_scriptParameters[name]=value;
          } else {
              Q_ASSERT("Unknown tag");
          }
      }

  } else {
    Q_ASSERT(0 && "invalid tag");
    return false;
  }
  return true;
}

const QVariantMap &GeneralParameters::scriptParameters() const
{
    return m_scriptParameters;
}

bool GeneralParameters::operator==(const GeneralParameters& other) const
{
    return  ApexParameters::operator==(other) &&
            m_bExitAfter == other.m_bExitAfter &&
            m_bAutoSave == other.m_bAutoSave &&
            m_bWaitForStart == other.m_bWaitForStart &&
            m_bAllowSkip == other.m_bAllowSkip &&
            m_bRunOutputTest == other.m_bRunOutputTest &&
            m_sOutputTestInput == other.m_sOutputTestInput &&
            m_scriptLibrary == other.m_scriptLibrary;
}

