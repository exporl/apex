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

#include "apexprocedureparameters.h"
#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;
using namespace apex::data;

#include "xml/xercesinclude.h"
using namespace xercesc;

#include <iostream>

#include <QStringList>


bool Choices::operator==(const Choices& other)
{
    return  std::operator==(*this, other) &&
            nChoices == other.nChoices;
}


ApexProcedureParameters::ApexProcedureParameters(DOMElement* p_paramElement)
        : ApexParameters( p_paramElement),
        m_presentations( -1 ),
        m_skip( 0 ),
        m_order( ORDER_SEQUENTIAL ),
        m_input_during_stimulus( false ),
        m_pause_between_stimuli(0),
        m_time_before_first_stimulus(0),
        m_uniquestandard(false)
{
    // FIXME: remove
}

ApexProcedureParameters::ApexProcedureParameters()
        : ApexParameters( ),
        m_presentations( -1 ),
        m_skip( 0 ),
        m_order( ORDER_SEQUENTIAL ),
        m_input_during_stimulus( false ),
        m_pause_between_stimuli(0),
        m_time_before_first_stimulus(0),
        m_uniquestandard(false)
{
}


ApexProcedureParameters::~ApexProcedureParameters() {}

const Choices& ApexProcedureParameters::GetChoices() const
{
	return m_choices;
}

int ApexProcedureParameters::GetPresentations() const
{
	return m_presentations;
}

QString ApexProcedureParameters::GetDefaultStandard() const
{
	return m_defaultstandard;
}

t_order ApexProcedureParameters::GetOrder() const
{
	return m_order;
}

bool ApexProcedureParameters::GetUniqueStandard() const
{
	return m_uniquestandard;
}

int ApexProcedureParameters::GetSkip() const
{
	return m_skip;
}

int ApexProcedureParameters::GetPauseBetweenStimuli() const
{
	return m_pause_between_stimuli;
}

int ApexProcedureParameters::GetTimeBeforeFirstStimulus() const
{
	return m_time_before_first_stimulus;
}

bool ApexProcedureParameters::GetInputDuringStimulus() const
{
	return m_input_during_stimulus;
}

void ApexProcedureParameters::setPresentations(int presentations)
{
    m_presentations = presentations;
}

void ApexProcedureParameters::setSkip(int skip)
{
    m_skip = skip;
}

void ApexProcedureParameters::setNumberOfChoices(int nb)
{
    m_choices.nChoices = nb;
}

void ApexProcedureParameters::setDefaultStandard(const QString& defStd)
{
    m_defaultstandard = defStd;
}

void ApexProcedureParameters::setOrder(t_order order)
{
    m_order = order;
}

void ApexProcedureParameters::setInputDuringStimulus(bool ids)
{
    m_input_during_stimulus = ids;
}

void ApexProcedureParameters::setTimeBeforeFirstStimulus(int tbfs)
{
    m_time_before_first_stimulus = tbfs;
}

#if 0
bool ApexProcedureParameters::SetParameter(const QString& p_name, const QString& /*p_id*/, const QString& p_value, DOMElement* d)
{
    if ( p_name == "presentations")
    {
        m_presentations = p_value.toInt();
    }
    else if ( p_name == "skip")
    {
        m_skip = p_value.toInt();
    }
    else if ( p_name == "choices")
    {
        m_choices.nChoices = p_value.toInt();

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
                if (pos>m_choices.nChoices) throw(ApexStringException("Element of choices/select can not be > choices itself"));
                m_choices.push_back(pos);
            }
        }
        else
        {
            for (int i=1; i<=m_choices.nChoices; ++i)
                m_choices.push_back(i);
        }

    }
    else if ( p_name == "defaultstandard")
    {
        m_defaultstandard = p_value;
    }
    else if ( p_name == "rev_for_mean")
    {
        // NOP
    }
    else if ( p_name == "order")
    {
        if ( p_value=="random")
            m_order = ORDER_RANDOM;
        else if ( p_value=="sequential")
            m_order = ORDER_SEQUENTIAL;
        else if ( p_value=="onebyone")      // only for multiprocedure
            m_order = ORDER_ONEBYONE;
        else
            std::cerr<< "ApexProcedureParameters::SetParameter: unknown order" << std::endl;

    }
    else if (p_name == "input_during_stimulus")
    {
        m_input_during_stimulus = (p_value == "true");
    }
    else if (p_name == "pause_between_stimuli")
    {
        m_pause_between_stimuli=p_value.toInt();
    }
    else if (p_name == "time_before_first_stimulus")
    {
        m_time_before_first_stimulus=p_value.toDouble();
    }
    else if (p_name == "uniquestandard")
    {
        m_uniquestandard= (p_value=="true");
    }
    else
    {
        //      std::cerr<< "ApexProcedureParameters::SetParameter: unknown tag " <<p_name << std::endl;
        return false;
    }
    return true;
}
#endif


bool ApexProcedureParameters::Parse( DOMElement * p_paramElement )
{

    return ApexParameters::Parse(p_paramElement);

}

bool ApexProcedureParameters::operator==(const ApexProcedureParameters& other) const
{
    return  ApexParameters::operator==(other) &&
            m_skip == other.m_skip &&
            m_choices == other.m_choices &&
            m_presentations == other.m_presentations &&
            m_defaultstandard == other.m_defaultstandard &&
            m_order == other.m_order &&
            m_input_during_stimulus == other.m_input_during_stimulus &&
            m_pause_between_stimuli == other.m_pause_between_stimuli &&
            m_time_before_first_stimulus == other.m_time_before_first_stimulus &&
            m_uniquestandard == other.m_uniquestandard;
}

