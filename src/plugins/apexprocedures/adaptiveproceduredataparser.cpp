/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "apexdata/procedure/adaptiveproceduredata.h"

#include "apextools/exceptions.h"

#include "adaptiveproceduredataparser.h"

namespace apex
{

namespace parser
{

AdaptiveProcedureDataParser::AdaptiveProcedureDataParser()
{
}

void AdaptiveProcedureDataParser::Parse(const QDomElement &p_base,
                                        data::AdaptiveProcedureData *p_data)
{
    ProcedureDataParser::Parse(p_base, p_data);
}

bool AdaptiveProcedureDataParser::SetParameter(const QString &p_name,
                                               const QString &p_id,
                                               const QString &p_value,
                                               const QDomElement &d,
                                               data::ProcedureData *data)
{
    Q_ASSERT(data);

    data::AdaptiveProcedureData *adaptiveData =
        dynamic_cast<data::AdaptiveProcedureData *>(data);

    Q_ASSERT(adaptiveData);

    if (p_name == "nUp") {
        adaptiveData->m_nUp = p_value.toInt();
    } else if (p_name == "nDown") {
        adaptiveData->m_nDown = p_value.toInt();
    } else if (p_name == "adapt_parameter") {
        adaptiveData->m_adapt_parameters.push_back(p_value);
    } else if (p_name == "start_value") {
        adaptiveData->m_startValue = p_value.toDouble();
    } else if (p_name == "min_value") {
        adaptiveData->setMinValue(p_value.toDouble());
    } else if (p_name == "max_value") {
        adaptiveData->setMaxValue(p_value.toDouble());
    } else if (p_name == "stop_after") {
        adaptiveData->m_nStop = p_value.toInt();
    } else if (p_name == "stop_after_type") {
        if (p_value == "reversals")
            adaptiveData->m_bStopAfterType =
                data::AdaptiveProcedureData::StopAfterReversals;
        else if (p_value == "trials")
            adaptiveData->m_bStopAfterType =
                data::AdaptiveProcedureData::StopAfterTrials;
        else if (p_value == "presentations")
            adaptiveData->m_bStopAfterType =
                data::AdaptiveProcedureData::StopAfterPresentations;
        else
            qFatal("Unknown stop_after_type");
    } else if (p_name == "larger_is_easier") {
        adaptiveData->m_bLargerIsEasier = QVariant(p_value).toBool();
    } else if (p_name == "repeat_first_until_correct") {
        adaptiveData->m_bFirstUntilCorrect = QVariant(p_value).toBool();
    } else if (p_name == "stepsizes") {
        ParseStepSizes(d, adaptiveData);
    } else {
        return ProcedureDataParser::SetParameter(p_name, p_id, p_value, d,
                                                 adaptiveData);
    }
    return true;
}

bool AdaptiveProcedureDataParser::ParseStepSizes(
    const QDomElement &p_base, data::AdaptiveProcedureData *data)
{
    Q_ASSERT(data);

    bool first = 1;

    for (QDomElement currentNode = p_base.firstChildElement();
         !currentNode.isNull();
         currentNode = currentNode.nextSiblingElement()) {
        const QString tag = currentNode.tagName();

        if (tag == "stepsize") {
            int begin = currentNode.attribute(QSL("begin")).toInt();
            float size = currentNode.attribute(QSL("size")).toFloat();
            QString direction = currentNode.attribute(QSL("direction"));

            if (first) {
                if (begin != 0) {
                    qCDebug(APEX_RS, "Error: first stepsize begin attrib "
                                     "should be 0, setting to 0");
                    begin = 0;
                }
                first = false;
            }

            if ("up" == direction) {
                data->m_upStepsizes[begin] = size;
            } else if ("down" == direction) {
                data->m_downStepsizes[begin] = size;
            } else {
                data->m_upStepsizes[begin] = size;
                data->m_downStepsizes[begin] = size;
            }

#ifdef PRINTPROCEDURE
            qCDebug(
                APEX_RS,
                QString("Stepsize - from %1 value %2").arg(begin).arg(size));
#endif
        } else if (tag == "change_after") {
            QString value = currentNode.text();
            if (value == "reversals") {
                data->m_changestepsize_type =
                    data::AdaptiveProcedureData::ChangeAfterReversals;
#ifdef PRINTPROCEDURE
                qCDebug(APEX_RS, "Changing stepsize after reversals");
#endif
            } else if (value == "trials") {
                data->m_changestepsize_type =
                    data::AdaptiveProcedureData::ChangeAfterTrials;
#ifdef PRINTPROCEDURE
                qCDebug(APEX_RS, "Changing stepsize after trials");
#endif
            } else {
                Q_ASSERT("invalid boolean");
                return false;
            }
        } else {
            qCDebug(APEX_RS,
                    "AdaptiveProcedureData::ParseStepSizes: unknown tag");
            throw 0;
        }
    }

    if (!data->m_upStepsizes.contains(0) || !data->m_downStepsizes.contains(0))
        throw ApexStringException("No stepsize defined with begin value 0");

    return true;
}

/**
 * Add errors/warnings to errorHandler
 * @param errorHandler
 * @return true if no fatal errors
 */
bool AdaptiveProcedureDataParser::CheckParameters(
    data::AdaptiveProcedureData *data)
{
    // FIXME: use this function
    bool result = true;

    if (data->m_defMinValue && data->m_defMaxValue &&
        data->m_minValue > data->m_maxValue) {
        throw ApexStringException(tr("Procedure: min_value > max_value"));
        result = false;
    }

    if (data->m_defMaxValue && data->m_startValue > data->m_maxValue) {
        throw ApexStringException(tr("Procedure: start_value > max_value"));
        result = false;
    }

    if (data->m_defMinValue && data->m_startValue < data->m_minValue) {
        throw ApexStringException(tr("start_value < min_value"));
        result = false;
    }

    if (data->m_bStopAfterType ==
            data::AdaptiveProcedureData::StopAfterPresentations &&
        (data->presentations() != data->m_nStop)) {
        throw ApexStringException(
            tr("<presentations> must be the same as <stop_after> "
               "if stop_after_type is presentations"));
        result = false;
    }

    return result;
}
}
}
