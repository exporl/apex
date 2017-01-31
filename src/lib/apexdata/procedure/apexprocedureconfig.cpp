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

#include "apexprocedureconfig.h"
#include "apextrial.h"
#include "apexprocedureparameters.h"

#include "xml/xercesinclude.h"
using namespace xercesc;
#include "xml/xmlkeys.h"
#include "xml/apexxmltools.h"
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using namespace apex::data;
#include "exceptions.h"
#include "apextools.h"

namespace apex
{


/**
 * p_parameters will be freed on destruction
 * trials will be parsed based on p_parent
 * @param p_parameters
 * @return
 */
ApexProcedureConfig::ApexProcedureConfig(ApexProcedureParameters* p_parameters/*,
    DOMElement* p_parent, ApexProcedureFactory* p_fact*/):
        //m_factory(p_fact),
        m_parameters(p_parameters)
{
//     if (p_parent)
//     {
//         ParseTrials(p_parent);
//         m_id=XMLutils::GetAttribute(p_parent,gc_sID);
//
//     }
}

ApexProcedureConfig::ApexProcedureConfig():
//        m_factory(0),
        m_parameters(0)
{
}



/**
 * p_trial will be freed on destruction
 * @param p_trial
 * @return
 */
bool ApexProcedureConfig::AddTrial(ApexTrial* p_trial)
{
    m_trials.push_back(p_trial);
    return true;
}

ApexTrial*  ApexProcedureConfig::GetTrial(const QString& p_name) const
{
    for (std::vector<ApexTrial*>::const_iterator p = m_trials.begin(); p != m_trials.end(); ++p)
    {
        if ( (*p)->GetID() == p_name)
            return *p;
    }

//        throw ApexTrialNotFoundException();
    throw ApexStringException("Trial not found: " + p_name);

}

ApexProcedureConfig::~ApexProcedureConfig()
{
    //delete m_parameters;      // is now auto_ptr
    std::vector<ApexTrial*>::iterator it = m_trials.begin();
    while ( it != m_trials.end() )
    {
        delete *it;
        it++;
    }
    m_trials.clear();
}

// void ApexProcedureConfig::SetProcedureFactory(ApexProcedureFactory* f)
// {
//     Q_ASSERT(f);
//     m_factory.reset(f);
// }
//
// ApexProcedureFactory* ApexProcedureConfig::GetProcedureFactory() const
// {
//     Q_ASSERT(m_factory.get());
//     return m_factory.get();
// }


}

// bool ApexProcedureConfig::ParseTrials( DOMElement * p_base )
// {
//     // parse trials block
//     DOMNode* trialNode = XMLutils::GetElementsByTagName( p_base, "trials" );
//     Q_ASSERT(trialNode);
//     Q_ASSERT(trialNode->getNodeType()== DOMNode::ELEMENT_NODE);
//
//     ApexTrialFactory trialFactory;
//     bool success=true;
//     // for each trial
//     for (DOMNode* currentNode=trialNode->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
//     {
//         Q_ASSERT(currentNode);
//         Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);
//
//         const QString tag = XMLutils::GetTagName( currentNode );
//         if (tag == "trial")
//         {
//             ApexTrial* newTrial = trialFactory.GetTrial(currentNode);
//             Q_ASSERT(newTrial);
//             /*m_procedureConfig->*/AddTrial(newTrial);
//         }
//         else
//         {
//             qDebug("ApexProcedureConfig::ParseTrials: unknown tag %s", qPrintable (tag));
//             success=false;
//         }
//
//     }
//
//     return success;
//
// }

void ApexProcedureConfig::SetParameters(ApexProcedureParameters * p)
{
    m_parameters.reset(p);
}

ApexProcedureParameters*  ApexProcedureConfig::GetParameters()
{
    return m_parameters.get();
};

const ApexProcedureParameters& ApexProcedureConfig::GetParameters() const
{
	return *m_parameters.get();
};

bool ApexProcedureConfig::operator==(const ApexProcedureConfig& other) const
{
    return  m_id == other.m_id &&
            ApexTools::haveSameContents(m_trials, other.m_trials) &&
            *m_parameters == *other.m_parameters;
}

