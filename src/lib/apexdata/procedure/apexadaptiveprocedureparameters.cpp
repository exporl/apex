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

#include "apexadaptiveprocedureparameters.h"
//#include "adaptiveprocedure.h"      // enum

#include <qstringlist.h>

#include "xml/xercesinclude.h"
using namespace xercesc;
#include "xml/apexxmltools.h"
using namespace apex::ApexXMLTools;
using namespace apex::data;

#include "apextools.h"

// #include "services/errorhandler.h"

#include <iostream>

ApexAdaptiveProcedureParameters::ApexAdaptiveProcedureParameters(DOMElement* p_paramElement)
        : ApexProcedureParameters(p_paramElement),
        m_defMinValue(false),
        m_defMaxValue(false),
        m_bFirstUntilCorrect(false),
        m_changestepsize_type(AFTER_TRIALS),
        m_bStopAfterType(STOP_ABOVE),
        m_nStop(1),
        m_bLargerIsEasier(true)
{
    // FIXME: remove
}

ApexAdaptiveProcedureParameters::ApexAdaptiveProcedureParameters()
        : ApexProcedureParameters(),
        m_defMinValue(false),
        m_defMaxValue(false),
        m_bFirstUntilCorrect(false),
        m_changestepsize_type(AFTER_TRIALS),
        m_bStopAfterType(STOP_ABOVE),
        m_nStop(1),
        m_bLargerIsEasier(true)
{
}


ApexAdaptiveProcedureParameters::~ApexAdaptiveProcedureParameters()
{
}

int ApexAdaptiveProcedureParameters::nUp() const
{
    return m_nUp;
}

int ApexAdaptiveProcedureParameters::nDown() const
{
    return m_nDown;
}

QStringList ApexAdaptiveProcedureParameters::adaptingParameters() const
{
    return m_adapt_parameters;
}

t_adaptParam ApexAdaptiveProcedureParameters::startValue() const
{
    return m_startValue;
}

int ApexAdaptiveProcedureParameters::stopAfterType() const
{
    return m_bStopAfterType;
}

QString ApexAdaptiveProcedureParameters::stopAfterTypeString() const
{
    switch (m_bStopAfterType)
    {
        case (STOP_REVERSALS): return "reversals";
        case (STOP_TRIALS)   : return "trials";
        case (STOP_ABOVE)    : return "presentations";

        default: qFatal("unknown stop-after type");
    }

    return QString(); //keep compiler happy
}

int ApexAdaptiveProcedureParameters::stopAfter() const
{
    return m_nStop;
}

t_adaptParam ApexAdaptiveProcedureParameters::minValue() const
{
    return m_minValue;
}

bool ApexAdaptiveProcedureParameters::hasMinValue() const
{
    return m_defMinValue;
}

t_adaptParam ApexAdaptiveProcedureParameters::maxValue() const
{
    return m_maxValue;
}

bool ApexAdaptiveProcedureParameters::hasMaxValue() const
{
    return m_defMaxValue;
}

bool ApexAdaptiveProcedureParameters::repeatFirstUntilCorrect() const
{
    return m_bFirstUntilCorrect;
}

void ApexAdaptiveProcedureParameters::setRepeatFirstUntilCorrect(const bool rfuc)
{
     m_bFirstUntilCorrect=rfuc;
}


bool ApexAdaptiveProcedureParameters::largerIsEasier() const
{
    return m_bLargerIsEasier;
}

void ApexAdaptiveProcedureParameters::setLargerIsEasier(bool value)
{
    m_bLargerIsEasier = value;
}

t_type ApexAdaptiveProcedureParameters::GetType() const
{
    return TYPE_ADAPTIVE;
}

const std::map<int,float>& ApexAdaptiveProcedureParameters::stepsizes() const
{
    return m_stepsizes;
}

int ApexAdaptiveProcedureParameters::changeStepsizeAfter() const
{
    return m_changestepsize_type;
}

QString ApexAdaptiveProcedureParameters::changeStepsizeAfterString() const
{
    switch (m_changestepsize_type)
    {
        case AFTER_TRIALS   : return "trials";
        case AFTER_REVERSALS: return "reversals";

        default: qFatal("unknown change-after type");
    }

    return QString();//keep compiler happy
}

void ApexAdaptiveProcedureParameters::addStepsize(int trial, float stepsize)
{
    m_stepsizes[trial] = stepsize;
}

void ApexAdaptiveProcedureParameters::setStartValue(t_adaptParam start)
{
    m_startValue = start;
}

void ApexAdaptiveProcedureParameters::setNUp(int n)
{
    m_nUp = n;
}

void ApexAdaptiveProcedureParameters::setNDown(int n)
{
    m_nDown = n;
}

void ApexAdaptiveProcedureParameters::addAdaptingParameter(QString param)
{
    Q_ASSERT(!param.isEmpty());
    m_adapt_parameters.append(param);
}

/*bool ApexAdaptiveProcedureParameters::SetParameter(const  QString & p_name,const  QString & p_id,const  QString & p_value, DOMElement* p_base)
{
    if ( ApexProcedureParameters::SetParameter(p_name, p_id, p_value,p_base) == true)
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
                    qDebug("Error: first stepsize begin attrib should be 0, setting to 0");
                    begin=0;
                }
                first=false;
            }

            if (m_stepsizes.find(begin) != m_stepsizes.end())
            {
                qDebug("Warning: duplicate stepsize found");
                ErrorHandler::Get().Add(tError(tError::Warning, "AdaptiveProcedureParameters", "Warning: duplicate stepsize found"));
                // FIXME: use generic error reporting mechanism
            }

            m_stepsizes[begin] = size;
#ifdef PRINTPROCEDURE
            qDebug(QString("Stepsize - from %1 value %2").arg(begin).arg(size));
#endif
        }
        else if (tag == "change_after")
        {
            QString value = XMLutils::GetFirstChildText(currentNode);
            if (value=="reversals")
            {
                m_changestepsize_type=AdaptiveProcedure::AFTER_REVERSALS;
#ifdef PRINTPROCEDURE
                qDebug("Changing stepsize after reversals");
#endif
            }
            else if (value=="trials")
            {
                m_changestepsize_type=AdaptiveProcedure::AFTER_TRIALS;
#ifdef PRINTPROCEDURE
                qDebug("Changing stepsize after trials");
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
            qDebug( "ApexAdaptiveProcedureParameters::ParseStepSizes: unknown tag");
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

bool ApexAdaptiveProcedureParameters::
        operator==(const ApexAdaptiveProcedureParameters& other) const
{
    if (m_defMinValue != other.m_defMinValue)
        return false;
    if (m_defMinValue && (m_minValue != other.m_minValue))
        return false;

    if (m_defMaxValue != other.m_defMaxValue)
        return false;
    if (m_defMaxValue && (m_maxValue != other.m_maxValue))
        return false;

    return  ApexProcedureParameters::operator==(other) &&
            m_nUp == other.m_nUp &&
            m_nDown == other.m_nDown &&
            m_startValue == other.m_startValue &&
            ApexTools::haveSameContents(m_adapt_parameters,
                                        other.m_adapt_parameters) &&
            m_nStop == other.m_nStop &&
            m_bLargerIsEasier == other.m_bLargerIsEasier &&
            m_bFirstUntilCorrect == other.m_bFirstUntilCorrect &&
            m_bStopAfterType == other.m_bStopAfterType &&
            m_stepsizes == other.m_stepsizes &&
            m_changestepsize_type == other.m_changestepsize_type;
}

