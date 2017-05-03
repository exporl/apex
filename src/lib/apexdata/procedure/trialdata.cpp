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

#include "trialdata.h"

#include <algorithm>
#include <list>

using namespace apex::data;

TrialData::TrialData()
{

}

TrialData::~TrialData()
{
}

void TrialData::SetAnswer( const QString & p_answer )
{
    m_answer = p_answer;
}

void TrialData::SetScreen( const QString & p_screen )
{
    m_screen = p_screen;
}

void TrialData::SetID(const QString& p_id) {
    m_ID=p_id;
}

void TrialData::AddStimulus( const QString & p_answer )
{
    m_stimuli.push_back(p_answer);
}

const QString  TrialData::GetStimulus( QString & p_name ) const
{
    if (m_stimuli.size() != 1)
            qCDebug(APEX_RS, "TrialData::GetStimulus: Warning: more than one stimulus found");

    tStimulusList::const_iterator p = std::find(m_stimuli.begin(), m_stimuli.end(), p_name);
    if ( p != m_stimuli.end() ) return (*p);
    else throw(1);          // FIXME
}

/**
 * @return the id of a random stimulus from this trial
 */
const QString TrialData::GetRandomStimulus( ) const
{
    if (m_stimuli.size() == 1)
            return *(m_stimuli.begin());

    tStimulusList::const_iterator p = m_stimuli.begin();
    //int rs = ApexTools::RandomRange(0, ((int)m_stimuli.size())-1 ); // stimulus number to be used
    int rs = mRandom.nextUInt(((int)m_stimuli.size()) ); // stimulus number to be used
    qCDebug(APEX_RS, "TrialData::GetRandomStimulus: Selecting random stimulus #%i", rs);
    for (int i=0; i<rs; ++i)
            ++p;
    Q_ASSERT(p!=m_stimuli.end());
    return *p;
}

/**
 * Returns the first stimulus' id, if there's only one, otherwise return random stimulus from the list
 * @return
 */
const QString TrialData::GetStimulus(  ) const
{
    if (m_stimuli.size()!=1) {
        //int pos = ApexTools::RandomRange((int)m_stimuli.size()-1) ;
        int pos = mRandom.nextUInt(((int)m_stimuli.size())) ;
        qCDebug(APEX_RS, "Returning random stimulus #%i", pos);
        tStimulusList::const_iterator it= m_stimuli.begin();
        for (int i=0; i<pos; ++i)
                ++it;
        return (*it);
    } else {
        return( m_stimuli.front() );
    }
}

/**
 * Returns all stimuli in a trial
 * Implemented with QStringList because QtScript does not handle typedefs well
 * @return
 */
const QStringList TrialData::GetStimulusList() const
{
    return m_stimuli;
}

const apex::tStimulusList::const_iterator TrialData::GetStimulusIterator( ) const
{
    return m_stimuli.begin();
}

const QString TrialData::GetAnswer( ) const
{
    return m_answer;
}

void TrialData::AddStandard( const QString & p_name )
{
    m_standards.push_back(p_name);
}

/**
 *
 * @return empty string if no standard is present
 */
const QString TrialData::GetRandomStandard( ) const
{
    if (m_standards.size()>1) {
        //int pos = ApexTools::RandomRange((int)m_standards.size()-1) ;
        int pos = mRandom.nextUInt((int)m_standards.size()) ;
        qCDebug(APEX_RS, "Returning random standard #%i", pos);
        tStimulusList::const_iterator it= m_standards.begin();
        for (int i=0; i<pos; ++i)
            ++it;
        return (*it);
    } else if (m_standards.size()==0) {
        return QString();
    } else {
        return( m_standards.front() );
    }
}

const QString TrialData::GetAnswerElement( ) const
{
//      if (! m_answer_element.isEmpty()) {
//              return m_answer_element;
//      }
//
//      // get answer element from screen
//      Screen* t = m_parent->GetScreen(m_screen);
//      Q_ASSERT( ! t->GetDefaultAnswerElement().isempty() );
//      return t->GetDefaultAnswerElement();
        return m_answer_element;
}

bool TrialData::operator==(const TrialData& other) const
{
    return  m_answer == other.m_answer &&
            m_answer_element == other.m_answer_element &&
            m_screen == other.m_screen &&
            m_ID == other.m_ID &&
            ApexTools::haveSameContents(m_standards, other.m_standards);
}
