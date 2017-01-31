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

#include "apexdata/procedure/proceduredata.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "correctorparser.h"
#include "proceduredataparser.h"

#include <QStringList>

using namespace apex::ApexXMLTools;
using namespace XERCES_CPP_NAMESPACE;

namespace apex
{

namespace parser
{

void ProcedureDataParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base,
           data::ProcedureData* p_data)
{
    Q_ASSERT (p_base);
    Q_ASSERT(p_data);

    for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        if (currentNode->getNodeType() == DOMNode::ELEMENT_NODE)
        {
            QString tag   = XMLutils::GetTagName( currentNode );
            QString id    = XMLutils::GetAttribute( currentNode, XMLKeys::gc_sID );
            QString value = XMLutils::GetFirstChildText( currentNode );

            bool result = SetParameter(tag, id, value, (DOMElement*) currentNode, p_data);

            if (!result)
                qCDebug(APEX_RS, "ApexParametersParser: could not parse tag %s",
                       qPrintable( tag ) );

        }
        else
        {
            Q_ASSERT(0);            // TODO
        }
    }
}

bool ProcedureDataParser::SetParameter(const QString p_name,
        const QString /*id*/, const QString p_value ,
        XERCES_CPP_NAMESPACE::DOMElement* d, data::ProcedureData* data)
{
    Q_ASSERT(data);

    if ( p_name == "presentations")
    {
        data->setPresentations(p_value.toInt());
    }
    else if ( p_name == "skip")
    {
        data->setSkip(p_value.toInt());
    }
    else if (p_name == "intervals")
    {
        QString count = ApexXMLTools::XMLutils::GetAttribute(d, "count");
        data->setNumberOfChoices(count.toInt());

        QString select = ApexXMLTools::XMLutils::GetAttribute(d, "select");
        if (!select.isEmpty())
        {
            QStringList slist = select.split(",");
            for (QStringList::const_iterator constIterator = slist.constBegin(); constIterator != slist.constEnd();
                    ++constIterator)
            {
                int pos;
                bool ok;
                pos=(*constIterator).toInt(&ok);
                if (!ok) throw(ApexStringException("Part of choices/select not integer"));
//                if (pos>param->m_choices.nChoices)
//                    throw(ApexStringException("Element of choices/select can not be > choices itself"));
                data->addStimulusInterval(pos-1);
            }
        }

        for(xercesc::DOMNode* alternative = d->getFirstChild();
            alternative != 0;
            alternative = alternative->getNextSibling())
        {
            Q_ASSERT(alternative->getNodeType() == xercesc::DOMNode::ELEMENT_NODE);
            Q_ASSERT(XMLutils::GetTagName(alternative) == "interval");

            QString number = XMLutils::GetAttribute(alternative, "number");
            QString value = XMLutils::GetAttribute(alternative, "element");

            Q_ASSERT(!number.isEmpty());
            Q_ASSERT(!value.isEmpty());

            bool ok;
            int interval = number.toInt(&ok) - 1;

            Q_ASSERT(ok);
            Q_ASSERT(interval >= 0);

            data->setInterval(interval, value);
        }

        if (!data->choices().isValid())
        {
             throw ApexStringException(tr("procedure/choices is invalid. "
                "Check whether each interval has an associated screen element"));
        }
    } else if(p_name == "corrector") {
        if(data->choices().hasMultipleIntervals()) {
            throw ApexStringException(tr("Corrector specified with >= 2 alternatives. This is probably unwanted."));
        }
        data->setCorrectorData(CorrectorParser().Parse(d));
    }
    else if ( p_name == "defaultstandard")
    {
        data->setDefaultStandard(p_value);
    }
    else if ( p_name == "rev_for_mean")
    {
        // NOP
    }
    else if ( p_name == "order")
    {
        if ( p_value=="random")
            data->setOrder(data::ProcedureData::RandomOrder);
        else if ( p_value=="sequential")
            data->setOrder(data::ProcedureData::SequentialOrder);
        else if ( p_value=="onebyone")      // only for multiprocedure
            data->setOrder(data::ProcedureData::OneByOneOrder);
        else
            throw ApexStringException("ProcedureDataParser::SetParameter: unknown order");

    }
    else if (p_name == "input_during_stimulus")
    {
        data->setInputDuringStimulus(XMLutils::xmlBool(p_value));
    }
    else if (p_name == "pause_between_stimuli")
    {
        data->setPauseBetweenStimuli(p_value.toInt());
    }
    else if (p_name == "time_before_first_trial")
    {
        data->setTimeBeforeFirstStimulus(p_value.toDouble());
    }
    else if (p_name == "uniquestandard")
    {
        data->setUniqueStandard(XMLutils::xmlBool(p_value));
    }
    else
    {

        return false;
    }
    return true;

}


}

}


