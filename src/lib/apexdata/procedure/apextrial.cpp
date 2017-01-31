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
 
#include "apextrial.h"
#include "apexanswer.h"
#include "exceptions.h"

#include "apextools.h"

#include <list>
#include <algorithm>

using namespace apex::data;

ApexTrial::ApexTrial()
{

}


ApexTrial::~ApexTrial()
{
}


void ApexTrial::SetAnswer( const ApexAnswer & p_answer )
{
        m_answer = p_answer;
}

void ApexTrial::SetScreen( const QString & p_screen )
{
        m_screen = p_screen;
}

/*void ApexTrial::SetStimulus( const QString & p_stimulus )
{
        m_stimulus = p_stimulus;
}*/

void ApexTrial::SetID(const QString& p_id) {
        m_ID=p_id;
}

void ApexTrial::AddStimulus( const QString & p_answer )
{
        m_stimuli.push_back(p_answer);
}

const QString  ApexTrial::GetStimulus( QString & p_name ) const
{
        if (m_stimuli.size() != 1)
                qDebug("ApexTrial::GetStimulus: Warning: more than one stimulus found");

        tStimulusList::const_iterator p = std::find(m_stimuli.begin(), m_stimuli.end(), p_name);
        if ( p != m_stimuli.end() ) return (*p);
        else throw(1);          // FIXME
}


/**
 * @return the id of a random stimulus from this trial
 */
const QString ApexTrial::GetRandomStimulus( ) const
{
        if (m_stimuli.size() == 1)
                return *(m_stimuli.begin());

        tStimulusList::const_iterator p = m_stimuli.begin();
        int rs = ApexTools::RandomRange(0, ((int)m_stimuli.size())-1 ); // stimulus number to be used
        qDebug("ApexTrial::GetRandomStimulus: Selecting random stimulus #%i", rs);
        for (int i=0; i<rs; ++i)
                ++p;
        //Q_ASSERT(p!=m_stimuli.end());
        if (p==m_stimuli.end())
            throw ApexStringException(tr("Could not find a stimulus in the current trial"));
        return *p;
}



/**
 * Returns the first stimulus' id, if there's only one, otherwise return random stimulus from the list
 * @return
 */
const QString ApexTrial::GetStimulus(  ) const
{
//    qDebug("ApexTrial::GetStimulus " );
        if (m_stimuli.size()!=1) {
                int pos = ApexTools::RandomRange((int)m_stimuli.size()-1) ;
                qDebug("Returning random stimulus #%i", pos);
                tStimulusList::const_iterator it= m_stimuli.begin();
                for (int i=0; i<pos; ++i)
                        ++it;
                return (*it);
        } else {
                return( m_stimuli.front() );
        }
}

const apex::tStimulusList::const_iterator ApexTrial::GetStimulusIterator( ) const
{
        return m_stimuli.begin();
}

const ApexAnswer ApexTrial::GetAnswer( ) const
{
        return m_answer;
}

const QString ApexTrial::answerString() const
{
    return m_answer.string();
}

void ApexTrial::AddStandard( const QString & p_name )
{
        //m_standard = p_name;
    m_standards.push_back(p_name);
}

/**
 *
 * @return empty string if no standard is present
 */
const QString ApexTrial::GetRandomStandard( ) const
{

    if (m_standards.size()>1) {
        int pos = ApexTools::RandomRange((int)m_standards.size()-1) ;
        qDebug ("Returning random standard #%i", pos);
        tStimulusList::const_iterator it= m_standards.begin();
        for (int i=0; i<pos; ++i)
            ++it;
        return (*it);
    } else if (m_standards.size()==0) {
        return QString();
    } else {
        return( m_standards.front() );
    }


        //return m_standard;
}



const QString ApexTrial::GetAnswerElement( ) const
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

bool ApexTrial::operator==(const ApexTrial& other) const
{
    return  m_answer == other.m_answer &&
            m_answer_element == other.m_answer_element &&
            m_screen == other.m_screen &&
            m_ID == other.m_ID &&
            ApexTools::haveSameContents(m_standards, other.m_standards);
}
