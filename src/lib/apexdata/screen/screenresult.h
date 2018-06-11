/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 4.                                               *
 *                                                                            *
 * APEX 4 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 4 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 4.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifndef _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREENRESULT_H_
#define _EXPORL_SRC_LIB_APEXDATA_SCREEN_SCREENRESULT_H_

#include "apextools/global.h"

#include <QDebug>
#include <QMap>
#include <QObject>
#include <QPointF>
#include <QString>

namespace apex
{

typedef std::map<QString, QString> stimulusParamsT;

/**
Map containing the answers of a certain presentation on screen

@author Tom Francart,,,
*/
class APEXDATA_EXPORT ScreenResult
{
public:
    ScreenResult();
    virtual ~ScreenResult();

    const QString toXML() const;

    virtual void clear();

    void SetStimulusParameter(const QString &parameter, const QString &value);
    const stimulusParamsT &GetStimulusParameters() const
    {
        return stimulusparams;
    };

    void setLastClickPosition(const QPointF &point);
    const QPointF &lastClickPosition() const;

private:
    stimulusParamsT stimulusparams; //! used to keep parameters set on the
                                    //! screen of the last trial (eg using a
                                    //! spinbox)
    QPointF mLastClickPosition;

    typedef QString KeyType;
    typedef QString ValueType;
    typedef QMap<KeyType, ValueType> Parent;
    Parent map;

    friend QDebug operator<<(QDebug dbg, const ScreenResult &screenResult);

public:
    typedef Parent::const_iterator const_iterator;

    Parent::const_iterator begin() const;
    Parent::const_iterator end() const;
    const ValueType value(const KeyType &key,
                          const ValueType &defaultValue = QString()) const;
    ValueType &operator[](const KeyType &key);
    const ValueType operator[](const KeyType &key) const;
    bool contains(const KeyType &key) const;
    const Parent get() const;
};

inline QDebug operator<<(QDebug out, const apex::ScreenResult &screenResult)
{
    return (out << screenResult.map);
}
}

#endif
