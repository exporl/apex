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

#include "filterstester.h"
#include "filterswriter.h"
#include "filter/filterparser.h"
#include "xml/apexxmltools.h"

using namespace apex::ApexXMLTools;

FiltersTester::~FiltersTester()
{
}

void FiltersTester::ParseData ( DOMElement* root )
{
        Q_ASSERT( XMLutils::GetTagName( root ) == "apex:filters" );

        apex::parser::FilterParser parser;

        for ( DOMNode* currentNode = root->getFirstChild(); currentNode != NULL;
                         currentNode = currentNode->getNextSibling() )
        {
                Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
                Q_ASSERT( XMLutils::GetTagName( currentNode ) == "filter" );

                QString id = XMLutils::GetAttribute( currentNode, "id" );
                filtersdata[id] = parser.ParseFilter( dynamic_cast<DOMElement*>( currentNode ), 0 );
        }
}

DOMElement* FiltersTester::WriteData ( DOMDocument* doc )
{
        apex::writer::FiltersWriter writer;
        return writer.addElement( doc, filtersdata );
}
