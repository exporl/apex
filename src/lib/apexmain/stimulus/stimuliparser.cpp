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

#include "apexdata/stimulus/stimulidata.h"
#include "apexdata/stimulus/stimulusdata.h"
#include "apexdata/stimulus/stimulusparameters.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "parser/scriptexpander.h"

#include "playmatrix.h"
#include "stimuliparser.h"

using apex::data::StimulusParameters;
using apex::data::StimuliData;
using apex::data::StimulusDatablocksContainer;
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using namespace XERCES_CPP_NAMESPACE;

#include <QDebug>


namespace apex {

                stimulus::PlayMatrix *CreatePlayMatrix( XERCES_CPP_NAMESPACE::DOMElement *p_base );


}

void apex::parser::StimuliParser::Parse(XERCES_CPP_NAMESPACE::DOMElement * p_base, data::StimuliData * c,
                                             QString scriptLibraryFile, const QVariantMap& scriptParameters)
{

    Q_ASSERT(c);
    currentData=c;

#ifndef NOSCRIPTEXPAND
        // find plugin stimuli and expand them
    for ( DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        const QString tag( XMLutils::GetTagName( currentNode ) );
        if (tag == "pluginstimuli") {
            ScriptExpander expander(scriptLibraryFile, scriptParameters, m_parent);
            expander.ExpandScript(currentNode, "getStimuli");
        }
    }
#endif


    for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName( currentNode );
        if (tag == "stimulus")
        {
            ParseStimulus((DOMElement*)currentNode);
        }
        else if (tag == "fixed_parameters")
        {
            // create QStringList from defined parameters
            data::FixedParameterList params;
            for (DOMNode* cur=currentNode->getFirstChild(); cur!=0; cur=cur->getNextSibling()) {
                QString id= XMLutils::GetAttribute(cur, "id");
                params.push_back(id);
            }

            c->setFixedParameters( params );
        }
        else
        {
            throw ApexStringException( "Unknown tag: " + tag );
        }

    }


    currentData=0;

}

apex::parser::StimuliParser::StimuliParser(QWidget* parent):
        currentData(0),
        m_parent(parent)
{
}

void apex::parser::StimuliParser::ParseStimulus(
        XERCES_CPP_NAMESPACE::DOMElement * p_base)
{
    Q_ASSERT( p_base );
    Q_ASSERT( p_base->getNodeType() == DOMNode::ELEMENT_NODE );

    data::StimulusData target;

    target.m_id = XMLutils::GetAttribute( p_base, gc_sID );

    for ( DOMNode* currentNode = p_base->getFirstChild() ; currentNode != 0 ; currentNode = currentNode->getNextSibling() )
    {
        Q_ASSERT( currentNode );
        if ( currentNode->getNodeType() != DOMNode::ELEMENT_NODE ) {
            if (  currentNode->getNodeType() == DOMNode::TEXT_NODE)
                continue;

            qCDebug(APEX_RS, "Node type: %d",  currentNode->getNodeType());
            qFatal("Invalid node type");
        }

        DOMElement* el = (DOMElement*) currentNode;

        const QString tag   = XMLutils::GetTagName( currentNode );
        const QString value = XMLutils::GetFirstChildText( currentNode );

        if (tag == "description")
        {
            target.m_description = value;
        }
        else if (tag == "datablocks")
        {
            //target.m_playMatrix = CreatePlayMatrix( el );  // FIXME
            target.m_datablocksContainer = CreateDatablocksContainer(el);
            //qCDebug(APEX_RS) << target.m_datablocksContainer.toString();
        }
        else if (tag == "variableParameters")
        {
            target.m_varParams = CreateStimulusParameters(el);
        }
        else if (tag == "fixedParameters")
        {
            target.m_fixParams = CreateStimulusParameters(el);
        }
        else
        {
            throw ApexStringException( "Unknown tag: " + tag );
        }
    }

//     if (! target.m_playMatrix) {
//         target.m_playMatrix = new PlayMatrix(0,0);
//     }

    // add to map
    currentData->insert(target.m_id, target);

    //qCDebug(APEX_RS, "Parsed stimulus %s", qPrintable(target.m_id));

}


StimulusParameters apex::parser::StimuliParser::CreateStimulusParameters( DOMElement * base )
{
    Q_ASSERT(base);

    StimulusParameters param;

    for (DOMNode* currentNode=base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode != 0);

        if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            DOMElement* el = static_cast<DOMElement*>(currentNode);

            const QString id    = XMLutils::GetAttribute( el, gc_sID );
            const QString value = XMLutils::NodeToString( el );           // can contain markup (eg fixed parameter with <b>)

            Q_ASSERT( !id.isEmpty() );

            param.insert(id,value);
        }
        else
            qFatal("Could not parse node");
    }

    return param;
}

StimulusDatablocksContainer apex::parser::StimuliParser::CreateDatablocksContainer(DOMElement* data)
{
    Q_ASSERT(data != NULL);

    StimulusDatablocksContainer::Type type;

    QString tag = XMLutils::GetTagName(data);

    if (tag == "datablocks")
        type = StimulusDatablocksContainer::DATABLOCKS;
    else if (tag == "sequential")
        type = StimulusDatablocksContainer::SEQUENTIAL;
    else if (tag == "simultaneous")
        type = StimulusDatablocksContainer::SIMULTANEOUS;
    else
        qFatal("illegal tag in stimulus datablock");

    StimulusDatablocksContainer container(type);

    DOMNode *child;
    for (child = data->getFirstChild(); child != NULL;
         child = child->getNextSibling())
    {
        Q_ASSERT( child->getNodeType() == DOMNode::ELEMENT_NODE );
        QString tag = XMLutils::GetTagName(child);

        if (tag == "datablock") {
            StimulusDatablocksContainer datablock(
                    StimulusDatablocksContainer::DATABLOCK);
            datablock.id = XMLutils::GetAttribute(child, "id");
            container.append(datablock);
        } else if (tag == "sequential") {
            DOMElement *element = dynamic_cast<DOMElement*>(child);
            Q_ASSERT_X(element, Q_FUNC_INFO, "Xerces-C++ dynamic cast failed");
            container.append(CreateDatablocksContainer(element));
        } else if (tag == "simultaneous") {
            DOMElement *element = dynamic_cast<DOMElement*>(child);
            Q_ASSERT_X(element, Q_FUNC_INFO, "Xerces-C++ dynamic cast failed");
            container.append(CreateDatablocksContainer(element));
        } else {
            qFatal("illegal tag in stimulus datablock");
        }
    }

    return container;
}


// namespace apex {
//
// PlayMatrix * CreatePlayMatrix( DOMElement *p_base )
// {
//     stimulus::PlayMatrixCreator myC;
//   PlayMatrix* p = myC.mf_pCreateMatrix( p_base );
//
// #ifdef PRINTPLAYMATRIX
//   PlayMatrixCreator::sf_DoDisplay( p );
// #endif
//
//   return p;
// }
//
// }

