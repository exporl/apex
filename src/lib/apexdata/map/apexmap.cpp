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

#include "apexmap.h"

namespace apex
{

namespace data
{

struct ApexMapPrivate
{
    ApexMapPrivate();

    int nbElec;             /// number of electrodes
    ChannelMap defaultMap;  /// default parameters, when not defined in child map
};

ApexMapPrivate::ApexMapPrivate() : nbElec(0)
{
}

ApexMap::ApexMap() : d(new ApexMapPrivate())
{
    // to be used as powerup stimulus
    ChannelMap dummyMap;
    dummyMap.setComfortLevel(0);
    dummyMap.setThresholdLevel(0);
    dummyMap.setStimulationElectrode(0);
    dummyMap.setChannelNumber(0);
    insert(0, dummyMap);
}


ApexMap::ApexMap(const ApexMap& other) : QMap<int, ChannelMap>(other),
                                         d(new ApexMapPrivate(*other.d))
{

}


ApexMap::~ApexMap()
{
    delete d;
}

ChannelMap ApexMap::defaultMap() const
{
    return d->defaultMap;
}

void ApexMap::setDefaultMap(const ChannelMap& map)
{
    d->defaultMap = map;
}

int ApexMap::numberOfElectrodes() const
{
    return d->nbElec;
}

void ApexMap::setNumberOfElectrodes(int nb)
{
    d->nbElec = nb;
}

const QString ApexMap::toXml() const
{
    QString result;

    // show default map
    result += d->defaultMap.toXml();

    // iterate over channelmaps
    // don't show map 0
    for (const_iterator it = begin(); it != end(); ++it) {
        if (it.key() == 0)
            continue;
        result+= "<channelmap id=\"";
        result+= QString::number(it.key());
        result+= "\">\n";
        result+= it.value().toXml();
        result+= "</channelmap>\n";
    }

    return result;
}

//FIXME false is never returned...
bool ApexMap::isComplete() const
{
    const unsigned electrodes = 22;
    // The first array entry is used for the powerup
    bool okarray[electrodes + 1];
    okarray[0] = true;
    for (unsigned i = 0; i < electrodes; ++i)
        okarray[i + 1] = false;

    for (const_iterator it = begin(); it != end(); ++it) {
        if (okarray[it.key()] && it.key()!=0)
            qCDebug(APEX_RS, "Error: duplicate map entry: %i", it.key());
        okarray[it.key()] = true;
    }

    for (unsigned i = 0; i < electrodes; ++i)
        if (!okarray[i + 1])
            throw ApexStringException(tr("Unknown map channel: %1").arg(i + 1));

    return true;
}

ApexMap& ApexMap::operator=(const ApexMap& other)
{
    if (this != &other)
    {
        QMap<int, ChannelMap>::operator=(other);
        *d = *other.d;
    }

    return *this;
}

bool ApexMap::operator==(const ApexMap& other)
{
    return QMap<int, ChannelMap>::operator==(other) &&
           d->defaultMap == other.d->defaultMap &&
           d->nbElec == other.d->nbElec;
}

}//ns data
}//ns apex
