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

#ifndef APEXAPEXADAPTIVEPROCEDUREPARAMETERS_H
#define APEXAPEXADAPTIVEPROCEDUREPARAMETERS_H

//from libdata
#include "procedure/apexprocedureparameters.h"

#include <map>
#include <QStringList>

namespace apex
{
class ApexAdaptiveProcedure;
class AdaptiveProcedure;
namespace parser
{
class ApexAdaptiveProcedureParametersParser;
}
namespace data
{

typedef double t_adaptParam;            // type of parameter to adapt

/**
@author Tom Francart,,,
*/
class APEXDATA_EXPORT ApexAdaptiveProcedureParameters : public data::ApexProcedureParameters
{
        enum {STOP_REVERSALS, STOP_TRIALS, STOP_ABOVE };

    public:
        ApexAdaptiveProcedureParameters(XERCES_CPP_NAMESPACE::DOMElement* p_paramElement);
        ApexAdaptiveProcedureParameters();

        enum ChangeStepsize {AFTER_TRIALS, AFTER_REVERSALS};

        //bool CheckParameters();

        ~ApexAdaptiveProcedureParameters();

        int nUp() const;
        int nDown() const;
        QStringList adaptingParameters() const;
        t_adaptParam startValue() const;
        int stopAfterType() const;
        QString stopAfterTypeString() const;
        int stopAfter() const;
        t_adaptParam minValue() const;
        bool hasMinValue() const;
        t_adaptParam maxValue() const;
        bool hasMaxValue() const;
        bool repeatFirstUntilCorrect() const;
        void setRepeatFirstUntilCorrect(const bool rfuc);

        bool largerIsEasier() const;
        void setLargerIsEasier(bool value);

        const std::map<int,float>& stepsizes() const;
        int changeStepsizeAfter() const;
        QString changeStepsizeAfterString() const;
        void addStepsize(int trial, float stepsize);
        void setStartValue(t_adaptParam start);
        void setNUp(int n);
        void setNDown(int n);
        void addAdaptingParameter(QString param);

        virtual data::t_type GetType() const;

        friend class apex::ApexAdaptiveProcedure;
        friend class parser::ApexAdaptiveProcedureParametersParser;

        bool operator==(const ApexAdaptiveProcedureParameters& other) const;

    private:
        //virtual bool SetParameter(const QString& p_name, const QString& p_id, const QString& p_value, XERCES_CPP_NAMESPACE::DOMElement*);
        //bool ParseStepSizes(XERCES_CPP_NAMESPACE::DOMElement* p_base);

        int m_nUp;
        int m_nDown;
        t_adaptParam m_startValue;

        t_adaptParam m_minValue;                // minimum value of t_adaptParam, important in case of adaptation of a variable parameter
        t_adaptParam m_maxValue;
        bool m_defMinValue;
        bool m_defMaxValue;

        QStringList m_adapt_parameters;   //! list of parameters to be adapted.
        // only the first can be a fixed parameter

        bool m_bFirstUntilCorrect;      // repeat the first trial untill the answer is correct
        int m_changestepsize_type;
        int m_bStopAfterType;           // stop after m_nStop reversals or m_nStop trials?

        int m_nStop;            // number of reversals before stop
        bool m_bLargerIsEasier;



        std::map<int,float> m_stepsizes;


};

}
}

#endif
