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
 
#include "apexscreenresult.h"

namespace apex
{

ApexScreenResult::ApexScreenResult()
{
}


ApexScreenResult::~ApexScreenResult()
{
}


}

const QString apex::ApexScreenResult::toXML( ) const
{
    QString temp = "<screenresult>\n";

    for (const_iterator p=begin(); p!=end(); ++p)
    {
        // do not report stuff for buttons and labels
        temp+="\t<element id=\"" + p.key() + "\">" + p.value() + "</element>\n";
    }

    temp += "</screenresult>";

    return temp;
}



void apex::ApexScreenResult::clear( )
{
    QMap<QString,QString>::clear();
    stimulusparams.clear();
}

void apex::ApexScreenResult::SetStimulusParameter( const QString & parameter, const QString & value )
{
    stimulusparams[parameter]=value;
}
