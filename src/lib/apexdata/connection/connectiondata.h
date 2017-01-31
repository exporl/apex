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

#ifndef __CONNECTIONDATA_H_
#define __CONNECTIONDATA_H_

#include <QVector>
#include <QString>

#include "global.h"

namespace apex
{
namespace data
{

struct ConnectionDataPrivate;

enum MatchType
{
    MATCH_NAME,
    MATCH_REGEXP,
    MATCH_WILDCARD
};

class APEXDATA_EXPORT ConnectionData
{
    public:

        ConnectionData();
        ConnectionData(const ConnectionData& other);
        ~ConnectionData();

        //getters
        MatchType matchType() const;
        const QString& fromId() const;
        const QString& fromChannelId() const;
        int fromChannel() const;
        const QString& toId() const;
        const QString& toChannelId() const;
        int toChannel() const;
        QString device() const;

        //setters
        void setMatchType(MatchType t);
        void setFromId(const QString& id);
        void setFromChannel(int channel, const QString& channelId = "");
        void setToId(const QString& id);
        void setToChannel(int channel, const QString& channelId = "");
        void setDevice(QString id);

        ConnectionData& operator=(const ConnectionData& other);
        bool operator==(const ConnectionData& other);
        bool duplicateOf(const ConnectionData& c);

    private:

        ConnectionDataPrivate* d;
};

class APEXDATA_EXPORT ConnectionsData : public QList<ConnectionData*>
{
    public:

        ConnectionsData();
        ~ConnectionsData();

        bool operator==(const ConnectionsData& other);

    private:

        //prevent copying
        ConnectionsData(const ConnectionsData&);
        ConnectionsData& operator=(const ConnectionsData&);

};

}//ns data
}//ns apex

#endif
