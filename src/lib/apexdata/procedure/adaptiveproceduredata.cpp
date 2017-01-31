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

#include "apextools/apextools.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"

#include "adaptiveproceduredata.h"

#include <QStringList>

using namespace xercesc;
using namespace apex::ApexXMLTools;
using namespace apex::data;

AdaptiveProcedureData::AdaptiveProcedureData()
        : ProcedureData(),
        m_defMinValue(false),
        m_defMaxValue(false),
        m_bFirstUntilCorrect(false),
        m_changestepsize_type(ChangeAfterTrials),
        m_bStopAfterType(StopAfterPresentations),
        m_nStop(1),
        m_bLargerIsEasier(true)
{
}


AdaptiveProcedureData::~AdaptiveProcedureData()
{
}

int AdaptiveProcedureData::nUp() const
{
    return m_nUp;
}

int AdaptiveProcedureData::nDown() const
{
    return m_nDown;
}

QStringList AdaptiveProcedureData::adaptingParameters() const
{
    return m_adapt_parameters;
}

adapting_parameter AdaptiveProcedureData::startValue() const
{
    return m_startValue;
}

AdaptiveProcedureData::StopAfter AdaptiveProcedureData::stopAfterType() const
{
    return m_bStopAfterType;
}

QString AdaptiveProcedureData::stopAfterTypeString() const
{
    switch (m_bStopAfterType)
    {
        case StopAfterReversals:     return "reversals";
        case StopAfterTrials:        return "trials";
        case StopAfterPresentations: return "presentations";

        default: qFatal("unknown stop-after type");
    }

    return QString(); //keep compiler happy
}

int AdaptiveProcedureData::stopAfter() const
{
    return m_nStop;
}

adapting_parameter AdaptiveProcedureData::minValue() const
{
    return m_minValue;
}

bool AdaptiveProcedureData::hasMinValue() const
{
    return m_defMinValue;
}

adapting_parameter AdaptiveProcedureData::maxValue() const
{
    return m_maxValue;
}

bool AdaptiveProcedureData::hasMaxValue() const
{
    return m_defMaxValue;
}

bool AdaptiveProcedureData::repeatFirstUntilCorrect() const
{
    return m_bFirstUntilCorrect;
}

void AdaptiveProcedureData::setRepeatFirstUntilCorrect(const bool rfuc)
{
     m_bFirstUntilCorrect=rfuc;
}


bool AdaptiveProcedureData::largerIsEasier() const
{
    return m_bLargerIsEasier;
}

void AdaptiveProcedureData::setLargerIsEasier(bool value)
{
    m_bLargerIsEasier = value;
}

AdaptiveProcedureData::Type AdaptiveProcedureData::type() const
{
        return AdaptiveType;
}

const QMap<int, float>& AdaptiveProcedureData::upStepsizes() const
{
    return m_upStepsizes;
}

const QMap<int, float>& AdaptiveProcedureData::downStepsizes() const
{
    return m_downStepsizes;
}

AdaptiveProcedureData::ChangeStepsizeAfter AdaptiveProcedureData::changeStepsizeAfter() const
{
    return m_changestepsize_type;
}

QString AdaptiveProcedureData::changeStepsizeAfterString() const
{
    switch (m_changestepsize_type)
    {
        case ChangeAfterTrials:    return "trials";
        case ChangeAfterReversals: return "reversals";

        default: qFatal("unknown change-after type");
    }

    return QString();//keep compiler happy
}

void AdaptiveProcedureData::addUpStepsize(int trial, float stepsize)
{
    m_upStepsizes[trial] = stepsize;
}

void AdaptiveProcedureData::addDownStepsize(int trial, float stepsize)
{
    m_downStepsizes[trial] = stepsize;
}

void AdaptiveProcedureData::setStopAfter(int stopAfter)
{
    m_nStop = stopAfter;
}

void AdaptiveProcedureData::setStartValue(const adapting_parameter start)
{
    m_startValue = start;
}

void AdaptiveProcedureData::setMinValue(const adapting_parameter minValue)
{
    m_defMinValue = true;
    m_minValue = minValue;
}

void AdaptiveProcedureData::setMaxValue(const adapting_parameter maxValue)
{
    m_defMaxValue = true;
    m_maxValue = maxValue;
}

void AdaptiveProcedureData::setNUp(int n)
{
    m_nUp = n;
}

void AdaptiveProcedureData::setNDown(int n)
{
    m_nDown = n;
}

void AdaptiveProcedureData::addAdaptingParameter(QString param)
{
    Q_ASSERT(!param.isEmpty());
    m_adapt_parameters.append(param);
}

/*bool ApexAdaptiveProcedureParameters::SetParameter(const  QString & p_name,const  QString & p_id,const  QString & p_value, DOMElement* p_base)
{
    if ( ProcedureParameters::SetParameter(p_name, p_id, p_value,p_base) == true)
        return true;

    if ( p_name == "nUp")
    {
        m_nUp = p_value.toInt();
    }
    else if ( p_name == "nDown")
    {
        m_nDown = p_value.toInt();
    }
    else if (p_name=="adapt_parameter")
    {
        //m_adapt_parameter = p_value;
        m_adapt_parameters.push_back(p_value);
    }
    else if (p_name=="start_value")
    {
        m_startValue = p_value.toDouble();
    }
    else if (p_name=="min_value")
    {
        m_minValue = p_value.toDouble();
        m_defMinValue=true;
    }
    else if (p_name=="max_value")
    {
        m_maxValue = p_value.toDouble();
        m_defMaxValue=true;
    }
    else if (p_name=="stop_after")
    {
        m_nStop = p_value.toInt();
    }
    else if (p_name=="stop_after_type")
    {
        if (p_value=="reversals")
            m_bStopAfterType = STOP_REVERSALS;
        else if (p_value=="trials")
            m_bStopAfterType = STOP_TRIALS;
        else if (p_value=="presentations")
            m_bStopAfterType = STOP_ABOVE;
        else
            Q_ASSERT("0");
    }
    else if (p_name=="larger_is_easier")
    {
        if (p_value=="true" || p_value=="1")
            m_bLargerIsEasier=true;
        else
            m_bLargerIsEasier=false;
    }
    else if (p_name=="repeat_first_until_correct")
    {
        if (p_value=="true" || p_value=="1")
            m_bFirstUntilCorrect=true;
        else
            m_bFirstUntilCorrect=false;
    }
    else if (p_name=="stepsizes")
    {
        ParseStepSizes(p_base);
    }
    else
    {
        std::cerr<< "ApexAdaptiveProcedureParameters::SetParameter: unknown tag " << p_name.toAscii().data()<< std::endl;
        return false;
    }
    return true;
}*/




/*bool ApexAdaptiveProcedureParameters::ParseStepSizes(DOMElement* p_base)
{

    bool first=1;

    for (DOMNode* currentNode=p_base->getFirstChild(); currentNode!=0; currentNode=currentNode->getNextSibling())
    {
        Q_ASSERT(currentNode);
        Q_ASSERT(currentNode->getNodeType() == DOMNode::ELEMENT_NODE);

        const QString tag = XMLutils::GetTagName( currentNode );


        if (tag == "stepsize")
        {
            int begin = XMLutils::GetAttribute(currentNode, "begin").toInt();
            float size = XMLutils::GetAttribute(currentNode, "size").toFloat();
            if (first)
            {
                if (begin!=0)
                {
                    qCDebug(APEX_RS, "Error: first stepsize begin attrib should be 0, setting to 0");
                    begin=0;
                }
                first=false;
            }

            if (m_stepsizes.find(begin) != m_stepsizes.end())
            {
                qCDebug(APEX_RS, "Warning: duplicate stepsize found");
                ErrorHandler::Get().Add(tError(tError::Warning, "AdaptiveProcedureParameters", "Warning: duplicate stepsize found"));
                // FIXME: use generic error reporting mechanism
            }

            m_stepsizes[begin] = size;
#ifdef PRINTPROCEDURE
            qCDebug(APEX_RS, QString("Stepsize - from %1 value %2").arg(begin).arg(size));
#endif
        }
        else if (tag == "change_after")
        {
            QString value = XMLutils::GetFirstChildText(currentNode);
            if (value=="reversals")
            {
                m_changestepsize_type=AdaptiveProcedure::AFTER_REVERSALS;
#ifdef PRINTPROCEDURE
                qCDebug(APEX_RS, "Changing stepsize after reversals");
#endif
            }
            else if (value=="trials")
            {
                m_changestepsize_type=AdaptiveProcedure::AFTER_TRIALS;
#ifdef PRINTPROCEDURE
                qCDebug(APEX_RS, "Changing stepsize after trials");
#endif
            }
            else
            {
                Q_ASSERT("invalid boolean");
                return false;
            }


        }
        else
        {
            qCDebug(APEX_RS, "ApexAdaptiveProcedureParameters::ParseStepSizes: unknown tag");
            throw 0;
        }
    }

    return true;
}*/


/**
 * Add errors/warnings to errorHandler
 * @param errorHandler
 * @return true if no fatal errors
 */
/*bool apex::ApexAdaptiveProcedureParameters::CheckParameters()
{
    bool result=true;

    if (m_defMinValue && m_defMaxValue)
        if (m_minValue > m_maxValue)
        {
            ErrorHandler::Get().AddError( "AdaptiveProcedureParameters", "min_value > max_value");
            result = false;
        }

    if (m_defMaxValue && m_startValue > m_maxValue)
    {
        ErrorHandler::Get().AddError( "AdaptiveProcedureParameters", "start_value > min_value");
        result = false;
    }

    if (m_defMinValue && m_startValue < m_minValue)
    {
        ErrorHandler::Get().AddError( "AdaptiveProcedureParameters", "start_value < min_value");
        result = false;
    }

    if (m_bStopAfterType==STOP_ABOVE && (m_presentations != m_nStop)) {

    }

    return result;
}*/

bool AdaptiveProcedureData::
        operator==(const AdaptiveProcedureData& other) const
{
    if (m_defMinValue != other.m_defMinValue)
        return false;
    if (m_defMinValue && (m_minValue != other.m_minValue))
        return false;

    if (m_defMaxValue != other.m_defMaxValue)
        return false;
    if (m_defMaxValue && (m_maxValue != other.m_maxValue))
        return false;

    return  m_nUp == other.m_nUp &&
            m_nDown == other.m_nDown &&
            m_startValue == other.m_startValue &&
            ApexTools::haveSameContents(m_adapt_parameters,
                                        other.m_adapt_parameters) &&
            m_nStop == other.m_nStop &&
            m_bLargerIsEasier == other.m_bLargerIsEasier &&
            m_bFirstUntilCorrect == other.m_bFirstUntilCorrect &&
            m_bStopAfterType == other.m_bStopAfterType &&
            m_upStepsizes == other.m_upStepsizes &&
            m_downStepsizes == other.m_downStepsizes &&
            m_changestepsize_type == other.m_changestepsize_type;
}

QString AdaptiveProcedureData::name() const
{
     return QLatin1String("apex:adaptiveProcedure");
}

