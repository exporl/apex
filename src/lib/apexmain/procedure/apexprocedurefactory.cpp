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

#include "apexprocedurefactory.h"
#include "apexprocedureparameters.h"
#include "apexprocedureconfig.h"

#include "apexadaptiveprocedurefactory.h"
#include "apexconstantprocedurefactory.h"
#include "apextrainingprocedurefactory.h"
#include "apexmultiprocedurefactory.h"
#include "pluginprocedurefactory.h"

#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;

#include "xml/xercesinclude.h"
using namespace xercesc;

namespace apex
{
/*
** Examples:

ApexProcedure* ApexProcedureFactory::MakeProcedure()
{
        return new ApexProcedure();
}




ApexProcedureConfig * ApexProcedureFactory::MakeProcedureConfig( ApexProcedureParameters * p_parameters )
{
        return new ApexProcedureConfig(p_parameters);
}
*/


ApexProcedureFactory* apex::ApexProcedureFactory::GetProcedureFactory ( const QString & myType )
{
	if ( myType=="apex:adaptiveProcedureType" )
		return new ApexAdaptiveProcedureFactory;
	else if ( myType=="apex:constantProcedureType" )
		return new ApexConstantProcedureFactory;
	else if ( myType=="apex:trainingProcedureType" )
		return new ApexTrainingProcedureFactory;
	else if ( myType=="apex:multiProcedureType" )
		return new ApexMultiProcedureFactory;
	else if ( myType=="apex:pluginProcedureType" )
		return new PluginProcedureFactory;

	else
		throw ( ApexException() );

}

}

DOMElement * apex::ApexProcedureFactory::GetParamElement ( DOMElement * p_base )
{
	DOMNode* parameterNode = XMLutils::GetElementsByTagName ( p_base, "parameters" );
	Q_ASSERT ( parameterNode );
	Q_ASSERT ( parameterNode->getNodeType() == DOMNode::ELEMENT_NODE );
	return ( DOMElement* ) parameterNode;
}

