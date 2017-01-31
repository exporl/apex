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

#include "connectiondata.h"

#include "apextools.h"

namespace apex
{

namespace data
{

struct ConnectionDataPrivate
{
    ConnectionDataPrivate();

    MatchType matchType;
    QString   fromId;        //!< the element where the connection originates
    QString   toId;          //!< the element where the connection ends
    unsigned  fromChannel;   //!< the originating channel
    unsigned  toChannel;     //!< the ending channel
    QString   fromChannelId; //!< optional ID, used when changing connections as a variable parameter
    QString   toChannelId;   //!< optional ID, used when changing connections as a variable parameter
    QString   deviceId;
};

}

}

using namespace apex::data;

//struct ConnectionDataPrivate


ConnectionDataPrivate::ConnectionDataPrivate() : matchType(MATCH_NAME),
                                                 fromChannel(0),
                                                 toChannel(0)
{
}


//class ConnectionData

ConnectionData::ConnectionData() : d(new ConnectionDataPrivate())
{
}


ConnectionData::ConnectionData(const ConnectionData& other) :
         d(new ConnectionDataPrivate(*other.d))
{
}

ConnectionData::~ConnectionData()
{
    delete d;
}



void ConnectionData::setMatchType(MatchType t)
{
    d->matchType = t;
}

MatchType ConnectionData::matchType() const
{
    return d->matchType;
}

const QString& ConnectionData::fromId() const
{
    return d->fromId;
}

void ConnectionData::setFromId(const QString& id)
{
    d->fromId = id;
}

const QString& ConnectionData::fromChannelId() const
{
    return d->fromChannelId;
}

unsigned ConnectionData::fromChannel() const
{
    return d->fromChannel;
}

void ConnectionData::setFromChannel(unsigned channel, const QString& channelId)
{
    d->fromChannel = channel;
    d->fromChannelId = channelId;
}

const QString& ConnectionData::toId() const
{
    return d->toId;
}

void ConnectionData::setToId(const QString& id)
{
    d->toId = id;
}

const QString& ConnectionData::toChannelId() const
{
    return d->toChannelId;
}

unsigned ConnectionData::toChannel() const
{
    return d->toChannel;
}

void ConnectionData::setToChannel(unsigned channel, const QString& channelId)
{
    d->toChannel = channel;
    d->toChannelId = channelId;
}

bool ConnectionData::duplicateOf(const ConnectionData& c)
{
    return (c.fromId() == d->fromId) &&
           (c.toId() == d->toId) &&
           (c.fromChannel() == d->fromChannel) &&
           (c.toChannel() == d->toChannel);
}


ConnectionData& ConnectionData::operator=(const ConnectionData& other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}


bool ConnectionData::operator==(const ConnectionData& other)
{
    return  d->fromId == other.d->fromId &&
            d->toId == other.d->toId &&
            d->fromChannel == other.d->fromChannel &&
            d->toChannel == other.d->toChannel &&
            d->fromChannelId == other.d->fromChannelId &&
            d->toChannelId == other.d->toChannelId &&
            d->deviceId == other.d->deviceId;
}

QString ConnectionData::device() const
{
    return d->deviceId;
}

void ConnectionData::setDevice(QString id)
{
    d->deviceId = id;
}

ConnectionsData::~ConnectionsData()
{
    for (int i = 0; i < size(); ++i)
        delete at(i);
}

bool ConnectionsData::operator==(const ConnectionsData& other)
{
    return ApexTools::haveSameContents(*this, other);
}
