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

#ifndef _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_PROCEDUREDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_PROCEDURE_PROCEDUREDATA_H_

#include "../corrector/correctordata.h"

#include "apextools/global.h"

#include "trialdata.h"

#include <QStringList>

#include <memory>
#include <vector>

namespace apex
{

namespace parser
{
    class ProcedureDataParser;
}

namespace data
{

/**
 * All interval numbers are 0 based
 */
class APEXDATA_EXPORT Choices
{
    public:
        Choices(bool deterministic=false);

        void setChoices(int c);
        int nChoices() const;
        bool operator==(const Choices& other) const;
        void addStimulusInterval(int i);

        //! return a random valid stimulus position, 0 based
        int randomPosition() const;

        const QStringList& intervals() const;

        /**
         * Get interval number for given element id
         */
        int interval(const QString& element) const;
        QString element(int interval) const;
        void setInterval(int number, QString id);
        //QList<int> unassociatedIntervals() const;
        QList<int> selectedIntervals() const;

        bool isValid() const;
        bool hasMultipleIntervals() const;

    private:

        mutable Random mRandom;
        QStringList mIntervals;
        int m_nChoices;
        QList<int> stimulusIntervals;
};

/**
Contains all configuration information for a procedure:
- proceduredata
- list of trials

proceduredata takes ownership of the trials

@author Tom Francart,,,
*/
class APEXDATA_EXPORT ProcedureData
{

    public:
        enum Order
        {
            RandomOrder,
            SequentialOrder,
            OneByOneOrder
        };

        enum Type
        {
            AdaptiveType,
            ConstantType,
            TrainingType,
            MultiType,
            PluginType
        };

        ProcedureData();
        virtual ~ProcedureData();

        //! we take ownership of the given trial
        virtual bool AddTrial(TrialData* p_trial);
        virtual TrialData* GetTrial(const QString& p_name) const;
        virtual const tTrialList& GetTrials() const {return m_trials;};


        void SetID(const QString& p_id);
        const QString& GetID() const;

        const Choices& choices() const;
        int presentations() const;
        QString defaultStandard() const /*{ return m_defaultstandard; }*/;
        Order order() const;
        bool uniqueStandard() const /*{ return m_uniquestandard; }*/;
        int skip() const;
        int pauseBetweenStimuli() const;
        int timeBeforeFirstStimulus() const;
        bool inputDuringStimulus() const;
        //const QString& GetID() const {return m_id;};
        data::CorrectorData* correctorData() const;

        void setPresentations(int presentations);
        void setSkip(int skip);
        void setNumberOfChoices(int nb);
        void setDefaultStandard(const QString& defStd);
        void setUniqueStandard(bool u);
        void setOrder(Order order);
        void setInputDuringStimulus(bool ids);
        void setTimeBeforeFirstStimulus(int tbfs);
        void setPauseBetweenStimuli(int pause);
        void addStimulusInterval(int i);
        void setInterval(int number, QString id);
        void setCorrectorData(data::CorrectorData* correctorData);

        virtual Type type() const = 0;
        virtual QString name() const = 0;         //! plugin name

        virtual bool operator==(const ProcedureData& other) const;

    protected:
        tTrialList m_trials;
        QString m_id;

    private:
        // parameters
        int m_presentations;
        int m_skip;
        //int m_choices;
        Choices m_choices;
        QString m_defaultstandard;
        Order m_order;
        bool m_input_during_stimulus;           // get user input during the stimulus output
        int m_pause_between_stimuli;
        double m_time_before_first_stimulus;
        bool m_uniquestandard;
        QScopedPointer<data::CorrectorData> m_correctorData;
};

}
}
#endif
