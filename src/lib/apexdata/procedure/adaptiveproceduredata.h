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

#ifndef _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_ADAPTIVEPROCEDUREDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_ADAPTIVEPROCEDUREDATA_H_

#include "proceduredata.h"

#include <QMap>
#include <QStringList>

namespace apex
{

class ApexAdaptiveProcedure;
class AdaptiveProcedure;

namespace parser
{
class AdaptiveProcedureDataParser;
}

namespace data
{

typedef double adapting_parameter;

class APEXDATA_EXPORT AdaptiveProcedureData : public data::ProcedureData
{
public:
    enum StopAfter {
        StopAfterReversals,
        StopAfterTrials,
        StopAfterPresentations
    };

    enum ChangeStepsizeAfter { ChangeAfterTrials, ChangeAfterReversals };

    AdaptiveProcedureData();

    // bool CheckParameters();

    ~AdaptiveProcedureData();

    int nUp() const;
    int nDown() const;
    QStringList adaptingParameters() const;
    adapting_parameter startValue() const;
    StopAfter stopAfterType() const;
    QString stopAfterTypeString() const;
    int stopAfter() const;
    adapting_parameter minValue() const;
    bool hasMinValue() const;
    adapting_parameter maxValue() const;
    bool hasMaxValue() const;
    bool repeatFirstUntilCorrect() const;
    void setRepeatFirstUntilCorrect(const bool rfuc);
    bool largerIsEasier() const;
    void setLargerIsEasier(bool value);
    const QMap<int, float> &upStepsizes() const;
    const QMap<int, float> &downStepsizes() const;
    ChangeStepsizeAfter changeStepsizeAfter() const;
    QString changeStepsizeAfterString() const;
    void addUpStepsize(int trial, float stepsize);
    void addDownStepsize(int trial, float stepsize);
    void setStopAfter(int stopAfter);
    void setStartValue(const adapting_parameter start);
    void setMinValue(const adapting_parameter minValue);
    void setMaxValue(const adapting_parameter maxValue);
    void setNUp(int n);
    void setNDown(int n);
    void addAdaptingParameter(QString param);

    virtual Type type() const;
    virtual QString name() const;

    friend class parser::AdaptiveProcedureDataParser;

    bool operator==(const AdaptiveProcedureData &other) const;

private:
    int m_nUp;
    int m_nDown;
    adapting_parameter m_startValue;

    adapting_parameter m_minValue; // minimum value of t_adaptParam, important
                                   // in case of adaptation of a variable
                                   // parameter
    adapting_parameter m_maxValue;
    bool m_defMinValue;
    bool m_defMaxValue;

    QStringList m_adapt_parameters; //! list of parameters to be adapted.
    // only the first can be a fixed parameter

    bool m_bFirstUntilCorrect; // repeat the first trial untill the answer is
                               // correct
    ChangeStepsizeAfter m_changestepsize_type;
    StopAfter
        m_bStopAfterType; // stop after m_nStop reversals or m_nStop trials?

    int m_nStop; // number of reversals before stop
    bool m_bLargerIsEasier;

    QMap<int, float> m_upStepsizes;
    QMap<int, float> m_downStepsizes;
};
}
}

#endif
