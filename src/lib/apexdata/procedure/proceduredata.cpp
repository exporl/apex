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
#include "apextools/exceptions.h"

#include "apextools/xml/apexxmltools.h"
#include "apextools/xml/xercesinclude.h"
#include "apextools/xml/xmlkeys.h"

#include "corrector/correctordata.h"

#include "proceduredata.h"
#include "trialdata.h"

using namespace xercesc;
using namespace apex::XMLKeys;
using namespace apex::ApexXMLTools;
using namespace apex::data;

namespace apex
{

Choices::Choices(bool deterministic) :
    m_nChoices(-1)
{
    if (deterministic)
        mRandom.setSeed(0);
}

void Choices::setChoices(int c)
{
    Q_ASSERT(c>0);
    mIntervals.reserve(c);
    for (int i = mIntervals.size(); i < c; ++i)
        mIntervals.append(QString());

    m_nChoices=c;
}

int Choices::nChoices() const
{
//    Q_ASSERT(nChoices!=-1);
    return m_nChoices;
}

int Choices::randomPosition() const
{
    if (stimulusIntervals.isEmpty() ) {
        return mRandom.nextUInt( m_nChoices );
         //return ApexTools::RandomRange(0, nChoices-1 );
    }
    int pos;
    while (1) {
        //pos= ApexTools::RandomRange(0, nChoices-1 );
        pos = mRandom.nextUInt( m_nChoices );
        if (stimulusIntervals.contains(pos))
            break;
    }
    return pos;
}

bool Choices::isValid() const
{
    // check whether each valid stimulus interval < nChoices
    Q_FOREACH (int interval, stimulusIntervals)
    {
        if (interval >= m_nChoices)
        {
            qCDebug(APEX_RS) << "interval" << interval << ">=" << m_nChoices;
            return false;
        }
    }

    return true;
}

bool Choices::hasMultipleIntervals() const
{
    return nChoices() > 1;
}

void Choices::addStimulusInterval(int i)
{
     stimulusIntervals.append(i);
}

 const QStringList& Choices::intervals() const
{
    return mIntervals;
}

void Choices::setInterval(int number, QString id)
{
    qCDebug(APEX_RS) << "setting interval" << number << "to" << id;
    for (int i = mIntervals.size(); i < number; ++i)
        mIntervals.append(QString());

    mIntervals[number]=id;
}


int Choices::interval(const QString& element) const
{
    int idx= mIntervals.indexOf(element);
    return idx;
}

QString Choices::element(int interval) const
{
    return mIntervals.value(interval);
}

/*QList<int> Choices::unassociatedIntervals() const
{

}*/

QList<int> Choices::selectedIntervals() const
{
    return stimulusIntervals;
}

bool Choices::operator==(const Choices& other) const
{
    return  nChoices() == other.nChoices() &&
        intervals() == other.intervals() &&
        stimulusIntervals == other.stimulusIntervals;
}

ProcedureData::ProcedureData() :
    m_presentations( -1 ),
    m_skip( 0 ),
    m_order( SequentialOrder ),
    m_input_during_stimulus( false ),
    m_pause_between_stimuli(0),
    m_time_before_first_stimulus(0),
    m_uniquestandard(false)
{
    m_correctorData.reset(new CorrectorData());
    m_correctorData->setType(data::CorrectorData::EQUAL);
}

/**
 * p_trial will be freed on destruction
 * @param p_trial
 * @return
 */
bool ProcedureData::AddTrial(TrialData* p_trial)
{
    m_trials.push_back(p_trial);
    return true;
}

TrialData*  ProcedureData::GetTrial(const QString& p_name) const
{
    for (tTrialList::const_iterator p = m_trials.begin(); p != m_trials.end(); ++p)
    {
        if ( (*p)->GetID() == p_name)
            return *p;
    }

    throw ApexStringException("Trial not found: " + p_name);
}

ProcedureData::~ProcedureData()
{
    tTrialList::iterator it = m_trials.begin();
    while ( it != m_trials.end() )
    {
        delete *it;
        it++;
    }
    m_trials.clear();
}

const Choices& ProcedureData::choices() const
{
    return m_choices;
}

int ProcedureData::presentations() const
{
    return m_presentations;
}

QString ProcedureData::defaultStandard() const
{
    return m_defaultstandard;
}

ProcedureData::Order ProcedureData::order() const
{
    return m_order;
}

bool ProcedureData::uniqueStandard() const
{
    return m_uniquestandard;
}

int ProcedureData::skip() const
{
    return m_skip;
}

int ProcedureData::pauseBetweenStimuli() const
{
    return m_pause_between_stimuli;
}

int ProcedureData::timeBeforeFirstStimulus() const
{
    return m_time_before_first_stimulus;
}

bool ProcedureData::inputDuringStimulus() const
{
    return m_input_during_stimulus;
}

CorrectorData* ProcedureData::correctorData() const
{
    return m_correctorData.data();
}

void ProcedureData::setPresentations(int presentations)
{
    m_presentations = presentations;
}

void ProcedureData::setSkip(int skip)
{
    m_skip = skip;
}

void ProcedureData::setNumberOfChoices(int nb)
{
    m_choices.setChoices(  nb );
}

void ProcedureData::setDefaultStandard(const QString& defStd)
{
    m_defaultstandard = defStd;
}

void ProcedureData::setUniqueStandard(const bool u)
{
    m_uniquestandard=u;
}

void ProcedureData::setOrder(Order order)
{
    m_order = order;
}

void ProcedureData::setInputDuringStimulus(bool ids)
{
    m_input_during_stimulus = ids;
}

void ProcedureData::setTimeBeforeFirstStimulus(int tbfs)
{
    m_time_before_first_stimulus = tbfs;
}

void ProcedureData::setPauseBetweenStimuli(int pause)
{
    m_pause_between_stimuli = pause;
}

void ProcedureData::addStimulusInterval(int i)
{
    m_choices.addStimulusInterval(i);
}

void ProcedureData::setInterval(int number, QString id)
{
    m_choices.setInterval(number, id);
}

void ProcedureData::setCorrectorData(data::CorrectorData* correctorData)
{
    m_correctorData.reset(correctorData);
}

#define ARE_EQUAL(a, b) \
    ((a.data() == 0 && b.data() == 0) ? true :\
    ((a.data() == 0 || b.data() == 0) ? false :\
    *a == *b))

bool ProcedureData::operator==(const ProcedureData& other) const
{
    return  m_id == other.m_id &&
            ApexTools::haveSameContents(m_trials, other.m_trials) &&
            m_skip == other.m_skip &&
            m_choices == other.m_choices &&
            m_presentations == other.m_presentations &&
            m_defaultstandard == other.m_defaultstandard &&
            m_order == other.m_order &&
            m_input_during_stimulus == other.m_input_during_stimulus &&
            m_pause_between_stimuli == other.m_pause_between_stimuli &&
            m_time_before_first_stimulus == other.m_time_before_first_stimulus &&
            m_uniquestandard == other.m_uniquestandard &&
            ARE_EQUAL(m_correctorData, other.m_correctorData);
}


#ifdef PRINTPROCEDURECONFIG
        void  ProcedureData::SetID(const QString& p_id) {
            qCDebug(APEX_RS, "ProcedureConfig: Set id: "+p_id); m_id=p_id;
        };
        const  ProcedureData::QString& GetID() const {
            qCDebug(APEX_RS, "ProcedureConfig: Get id: "+m_id); return m_id;
        };
#else
        void  ProcedureData::SetID(const QString& p_id) {
            m_id=p_id;
        };
        const QString& ProcedureData::GetID() const {
            return m_id;
        };
#endif

}
