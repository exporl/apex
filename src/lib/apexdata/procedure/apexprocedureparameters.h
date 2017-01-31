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

#ifndef APEXAPEXPROCEDUREPARAMETERS_H
#define APEXAPEXPROCEDUREPARAMETERS_H

#include "parameters/apexparameters.h"

//from libtools
#include "global.h"

#include <list>

namespace apex
{
class ApexProcedure;
namespace parser
{
class ApexProcedureParametersParser;
}

namespace data
{

typedef int t_order;
typedef int t_type;


class APEXDATA_EXPORT Choices: public std::list<int>
{
    public:
        Choices() : nChoices(-1) {};

        int nChoices;

        bool operator==(const Choices& other);

};

/**
@author Tom Francart,,,
*/
class APEXDATA_EXPORT ApexProcedureParameters: public ApexParameters
{
    public:
        ApexProcedureParameters(XERCES_CPP_NAMESPACE::DOMElement* p_paramElement);
        ApexProcedureParameters();

        ~ApexProcedureParameters();

        virtual bool Parse(XERCES_CPP_NAMESPACE::DOMElement* p_paramElement);

        const Choices& GetChoices() const /*{ return m_choices; }*/;
        int GetPresentations() const /*{ return m_presentations; }*/;
        QString GetDefaultStandard() const /*{ return m_defaultstandard; }*/;
        t_order GetOrder() const /*{ return m_order; }*/;
        bool GetUniqueStandard() const /*{ return m_uniquestandard; }*/;
        int GetSkip() const /*{ return m_skip; }*/;
        int GetPauseBetweenStimuli() const;
        int GetTimeBeforeFirstStimulus() const;
        bool GetInputDuringStimulus() const;
        //const QString& GetID() const {return m_id;};

        void setPresentations(int presentations);
        void setSkip(int skip);
        void setNumberOfChoices(int nb);
        void setDefaultStandard(const QString& defStd);
        void setOrder(t_order order);
        void setInputDuringStimulus(bool ids);
        void setTimeBeforeFirstStimulus(int tbfs);

        virtual t_type GetType() const = 0;

        virtual bool operator==(const ApexProcedureParameters& other) const;

        enum{ORDER_RANDOM, ORDER_SEQUENTIAL, ORDER_ONEBYONE};
        enum{TYPE_ADAPTIVE, TYPE_CONSTANT, TYPE_TRAINING, TYPE_MULTI, TYPE_PLUGIN};

    protected:
        //bool SetParameter(const QString& p_name, const QString& p_id, const QString& p_value, XERCES_CPP_NAMESPACE::DOMElement* p_base=0);


        friend class apex::ApexProcedure;
        friend class parser::ApexProcedureParametersParser;

    private:

        // parameters
        int m_presentations;
        int m_skip;
        //int m_choices;
        Choices m_choices;
        QString m_defaultstandard;
        t_order m_order;
        bool m_input_during_stimulus;           // get user input during the stimulus output
        int m_pause_between_stimuli;
        double m_time_before_first_stimulus;
        bool m_uniquestandard;
};

}
}

#endif
