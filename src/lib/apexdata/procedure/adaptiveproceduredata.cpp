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

#include "apextools/xml/xmltools.h"

#include "adaptiveproceduredata.h"

#include <QStringList>

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
    switch (m_bStopAfterType) {
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

bool AdaptiveProcedureData::operator==(const AdaptiveProcedureData& other) const
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

