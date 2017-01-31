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
 
#ifndef APEXAPEXSCREENRESULT_H
#define APEXAPEXSCREENRESULT_H

#include "global.h"

#include <QMap>
#include <QString>
#include <QObject>
#include <QPointF>

namespace apex {
    
    typedef std::map<QString,QString> stimulusParamsT;

/**
Map containing the answers of a certain presentation on screen

@author Tom Francart,,,
*/
class APEXDATA_EXPORT ScreenResult
{

public:
    typedef QMap<QString,QString>::const_iterator const_iterator;

    ScreenResult();
    virtual ~ScreenResult();

    const QString toXML() const;

    virtual void clear();

    void SetStimulusParameter(const QString& parameter, const QString& value);
    const stimulusParamsT& GetStimulusParameters() const { return stimulusparams;};

    void setLastClickPosition(const QPointF& point);
    const QPointF& lastClickPosition() const;

    const QMap<QString,QString>& map() const;
    QMap<QString,QString>& map();

    QString &	operator[] ( const QString & key );
    const QString operator[] ( const QString & key ) const;

    const_iterator find ( const QString & key ) const;
    const_iterator end () const;
    const_iterator begin () const;
        
    private:
        stimulusParamsT stimulusparams;     //! used to keep parameters set on the screen of the last trial (eg using a spinbox)
        QPointF mLastClickPosition;
        QMap<QString,QString> m_map;

};

}

#endif
