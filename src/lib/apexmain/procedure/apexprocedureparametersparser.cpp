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
 
#include "apexprocedureparametersparser.h"

//from libtools
#include "procedure/apexprocedureparameters.h"

#include <QStringList>

namespace apex
{

namespace parser
{



ApexProcedureParametersParser::ApexProcedureParametersParser()
{
}

void ApexProcedureParametersParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base,
           data::ApexProcedureParameters* p_parameters)
{
    ApexParametersParser::Parse(p_base, p_parameters);
}

bool ApexProcedureParametersParser::SetParameter(const QString p_name,
        const QString /*id*/, const QString p_value ,
        XERCES_CPP_NAMESPACE::DOMElement* d)
{
    Q_ASSERT(currentParameters);

    data::ApexProcedureParameters* param =
        dynamic_cast<data::ApexProcedureParameters*>(currentParameters);

    Q_ASSERT(param);

    if ( p_name == "presentations")
    {
        param->m_presentations = p_value.toInt();
    }
    else if ( p_name == "skip")
    {
        param->m_skip = p_value.toInt();
    }
    else if ( p_name == "choices")
    {
        param->m_choices.nChoices = p_value.toInt();

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
                if (pos>param->m_choices.nChoices)
                    throw(ApexStringException("Element of choices/select can not be > choices itself"));
                param->m_choices.push_back(pos);
            }
        }
        else
        {
            for (int i=1; i<=param->m_choices.nChoices; ++i)
                param->m_choices.push_back(i);
        }

    }
    else if ( p_name == "defaultstandard")
    {
        param->m_defaultstandard = p_value;
    }
/*    else if ( p_name == "rev_for_mean")
    {
        // NOP
    }*/
    else if ( p_name == "order")
    {
        if ( p_value=="random")
            param->m_order = data::ApexProcedureParameters::ORDER_RANDOM;
        else if ( p_value=="sequential")
            param->m_order = data::ApexProcedureParameters::ORDER_SEQUENTIAL;
        else if ( p_value=="onebyone")      // only for multiprocedure
            param->m_order = data::ApexProcedureParameters::ORDER_ONEBYONE;
        else
            throw ApexStringException("ApexProcedureParameters::SetParameter: unknown order"); 

    }
    else if (p_name == "input_during_stimulus")
    {
        param->m_input_during_stimulus = (p_value == "true");
    }
    else if (p_name == "pause_between_stimuli")
    {
        param->m_pause_between_stimuli=p_value.toInt();
    }
    else if (p_name == "time_before_first_trial")
    {
        param->m_time_before_first_stimulus=p_value.toDouble();
    }
    else if (p_name == "uniquestandard")
    {
        param->m_uniquestandard= (p_value=="true");
    }
    else
    {

        return false;
    }
    return true;

}


}

}


