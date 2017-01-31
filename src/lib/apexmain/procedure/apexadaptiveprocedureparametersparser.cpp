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

#include "apexadaptiveprocedureparametersparser.h"
#include "procedure/apexadaptiveprocedureparameters.h"
#include "procedure/adaptiveprocedure.h"
#include "services/errorhandler.h"

#include "xml/xercesinclude.h"
using namespace XERCES_CPP_NAMESPACE;
using namespace apex::ApexXMLTools;

namespace apex
{

namespace parser
{



    ApexAdaptiveProcedureParametersParser::ApexAdaptiveProcedureParametersParser()
{
}

void ApexAdaptiveProcedureParametersParser::Parse(XERCES_CPP_NAMESPACE::DOMElement* p_base,
           data::ApexAdaptiveProcedureParameters* p_parameters)
{
    ApexProcedureParametersParser::Parse(p_base, p_parameters);
}

bool ApexAdaptiveProcedureParametersParser::SetParameter(const QString p_name,
        const QString p_id, const QString p_value ,
        XERCES_CPP_NAMESPACE::DOMElement* d)
{
    Q_ASSERT(currentParameters);

    data::ApexAdaptiveProcedureParameters* param =
        dynamic_cast<data::ApexAdaptiveProcedureParameters*>(currentParameters);

    Q_ASSERT(param);

    if ( p_name == "nUp")
    {
        param->m_nUp = p_value.toInt();
    }
    else if ( p_name == "nDown")
    {
        param->m_nDown = p_value.toInt();
    }
    else if (p_name=="adapt_parameter")
    {
        param->m_adapt_parameters.push_back(p_value);
    }
    else if (p_name=="start_value")
    {
        param->m_startValue = p_value.toDouble();
    }
    else if (p_name=="min_value")
    {
        param->m_minValue = p_value.toDouble();
        param->m_defMinValue=true;
    }
    else if (p_name=="max_value")
    {
        param->m_maxValue = p_value.toDouble();
        param->m_defMaxValue=true;
    }
    else if (p_name=="stop_after")
    {
        param->m_nStop = p_value.toInt();
    }
    else if (p_name=="stop_after_type")
    {
        if (p_value=="reversals")
            param->m_bStopAfterType = data::ApexAdaptiveProcedureParameters::STOP_REVERSALS;
        else if (p_value=="trials")
            param->m_bStopAfterType =  data::ApexAdaptiveProcedureParameters::STOP_TRIALS;
        else if (p_value=="presentations")
            param->m_bStopAfterType =  data::ApexAdaptiveProcedureParameters::STOP_ABOVE;
        else
            Q_ASSERT("0");
    }
    else if (p_name=="larger_is_easier")
    {
        if (p_value=="true" || p_value=="1")
            param->m_bLargerIsEasier=true;
        else
            param->m_bLargerIsEasier=false;
    }
    else if (p_name=="repeat_first_until_correct")
    {
        if (p_value=="true" || p_value=="1")
            param->m_bFirstUntilCorrect=true;
        else
            param->m_bFirstUntilCorrect=false;
    }
    else if (p_name=="stepsizes")
    {
        ParseStepSizes(d);
    }
    else
    {
        return ApexProcedureParametersParser::SetParameter(p_name,
                p_id, p_value, d);
    }
    return true;



}



bool ApexAdaptiveProcedureParametersParser::ParseStepSizes(
        XERCES_CPP_NAMESPACE::DOMElement* p_base)
{

    Q_ASSERT(currentParameters);

    data::ApexAdaptiveProcedureParameters* param =
            dynamic_cast<data::ApexAdaptiveProcedureParameters*>(currentParameters);

    Q_ASSERT(param);

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

            if (param->m_stepsizes.find(begin) != param->m_stepsizes.end())
            {
                qDebug("Warning: duplicate stepsize found");
                ErrorHandler::Get().addItem(StatusItem(StatusItem::WARNING, "AdaptiveProcedureParameters", "Warning: duplicate stepsize found"));
                // FIXME: use generic error reporting mechanism
            }

            param->m_stepsizes[begin] = size;
#ifdef PRINTPROCEDURE
            qDebug(QString("Stepsize - from %1 value %2").arg(begin).arg(size));
#endif
        }
        else if (tag == "change_after")
        {
            QString value = XMLutils::GetFirstChildText(currentNode);
            if (value=="reversals")
            {
                param->m_changestepsize_type=data::ApexAdaptiveProcedureParameters::AFTER_REVERSALS;
#ifdef PRINTPROCEDURE
                qDebug("Changing stepsize after reversals");
#endif
            }
            else if (value=="trials")
            {
                param->m_changestepsize_type=data::ApexAdaptiveProcedureParameters::AFTER_TRIALS;
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
}


/**
 * Add errors/warnings to errorHandler
 * @param errorHandler
 * @return true if no fatal errors
 */
bool ApexAdaptiveProcedureParametersParser::CheckParameters(
        data::ApexAdaptiveProcedureParameters* param)
{
    // FIXME: use this function
    bool result=true;

    if (param->m_defMinValue && param->m_defMaxValue)
        if (param->m_minValue > param->m_maxValue)
    {
        ErrorHandler::Get().addError( "AdaptiveProcedureParameters",
                          tr("min_value > max_value"));
        result = false;
    }

    if (param->m_defMaxValue && param-> m_startValue > param->m_maxValue)
    {
        ErrorHandler::Get().addError( "AdaptiveProcedureParameters",
                          tr("start_value > min_value"));
        result = false;
    }

    if (param->m_defMinValue && param->m_startValue < param->m_minValue)
    {
        ErrorHandler::Get().addError( "AdaptiveProcedureParameters",
                          tr("start_value < min_value"));
        result = false;
    }

    if (param->m_bStopAfterType==data::ApexAdaptiveProcedureParameters::STOP_ABOVE &&
        (param->GetPresentations() != param->m_nStop)) {
        ErrorHandler::Get().addError( "AdaptiveProcedureParameters",
                          tr("<presentations> must be the same as <stop_after> "
                                  "if stop_after_type is presentations"));
        result = false;
    }

    return result;
}



}

}


