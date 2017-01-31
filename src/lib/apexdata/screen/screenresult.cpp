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

#include "screenresult.h"

namespace apex
{

ScreenResult::ScreenResult()
{
}


ScreenResult::~ScreenResult()
{
}


}

const QString apex::ScreenResult::toXML( ) const
{
     QString temp = "<screenresult";

    if ( mLastClickPosition.x() >= 0) {
        temp += QString(" xclickposition=\"%1\" yclickposition=\"%2\"" )
                .arg(mLastClickPosition.x()).arg(mLastClickPosition.y());
    }

    temp += ">\n";

    for (const_iterator p=begin(); p!=end(); ++p)
    {
        // do not report stuff for buttons and labels
        temp+="\t<element id=\"" + p.key() + "\">" + p.value() + "</element>\n";
    }

    temp += "</screenresult>";

    return temp;
}



void apex::ScreenResult::clear( )
{
    map.clear();
    stimulusparams.clear();
}

void apex::ScreenResult::SetStimulusParameter( const QString & parameter, const QString & value )
{
    stimulusparams[parameter]=value;
}

void apex::ScreenResult::setLastClickPosition(const QPointF& point)
{
    mLastClickPosition = point;
}

const QPointF&  apex::ScreenResult::lastClickPosition() const
{
    return mLastClickPosition;
}

apex::ScreenResult::Parent::const_iterator apex::ScreenResult::begin() const {
    return map.begin();
}

apex::ScreenResult::Parent::const_iterator apex::ScreenResult::end() const {
    return map.end();
}

const apex::ScreenResult::ValueType apex::ScreenResult::value (const KeyType& key, const ValueType& defaultValue) const {
    return map.value(key, defaultValue);
}

apex::ScreenResult::ValueType& apex::ScreenResult::operator[](const KeyType& key) {
    return map[key];
}

const apex::ScreenResult::ValueType apex::ScreenResult::operator[](const KeyType& key) const {
    return map[key];
}

bool apex::ScreenResult::contains(const KeyType& key) const {
    return map.contains(key);
}

const apex::ScreenResult::Parent apex::ScreenResult::get() const {
    return map;
}
