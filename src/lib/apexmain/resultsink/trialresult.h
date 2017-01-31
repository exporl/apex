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
 
#ifndef APEXTRIALRESULT_H
#define APEXTRIALRESULT_H

#include <qdatetime.h>
#include <qstring.h>

#include "screen/screenresult.h"
#include "procedure/adaptiveprocedure.h"

namespace apex {


/**
Contains the complete result of a single trial

@author Tom Francart,,,
*/
class TrialResult{
public:
    TrialResult();

    ~TrialResult();
        
    public:                 // we make it a simple struct
                
    QString name;
    QString stimulus;
    QString extra;                          // extra xml data
    ScreenResult screenResult;
    QDateTime timestamp;
    bool correctorResult;
    int answerTime;                         // # of ms before answer
    int correctAnswer;                      // the correct answer if defined by procedure
    data::t_adaptParam targetParam;
    bool defTargetParam;            // is target parameter defined?

    QString toXML() const;

};

}

#endif
