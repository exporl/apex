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

#include "apextools/status/statusreporter.h"

#include "stimulidata.h"

#include <QMap>
#include <QStringList>

namespace apex
{

namespace data
{

void StimuliData::setFixedParameters( FixedParameterList p)
{
    m_fixedParameters=p;
}


bool StimuliData::CheckFixedParameters (StatusReporter* err) {


    bool result=true;
    for (int i=0; i<m_fixedParameters.size(); ++i) {
        for ( QMap<QString,StimulusData>::const_iterator it=begin();
              it!=end(); ++it) {
            if (! it.value().GetFixedParameters().
                  contains(m_fixedParameters.at(i))) {
                err->addError("StimuliData", QString("Fixed parameter %1 not defined in stimulus %2").arg(m_fixedParameters.at(i)).arg(it.key()));
                result=false;
            }
        }
    }


    return result;


   /*std::map<QString,QString>::const_iterator i = m_fixedParameters->begin();
    for (; i!=m_fixedParameters->end(); ++i)
    {
        //qCDebug(APEX_RS, "Checking parameter %s", (*i).first.ascii() );

        std::map<QString, Stimulus*>::const_iterator j = m_stimuli.begin();
        bool tempresult=true;
        for (; j!=m_stimuli.end(); ++j)
        {
            if ( ! (*j).second->GetFixParameters()->HasParameter((*i).first)  )
            {
                m_progress.AddError( "CheckFixedParameters", QString("Error: fixed parameter %1 not found in stimulus %2").arg( (*i).first ).arg((*j).second->GetID()) );
                tempresult=false;
            }
        }
        if (tempresult==false)
        {
            return false;
        }
    }

    return true;*/

}



const FixedParameterList & StimuliData::GetFixedParameters() const
{
    return m_fixedParameters;
}

bool StimuliData::operator==(const StimuliData& other) const
{
    return  QMap<QString,StimulusData>::operator==(other) &&
            ApexTools::haveSameContents(m_fixedParameters, other.m_fixedParameters);
}


}

}
