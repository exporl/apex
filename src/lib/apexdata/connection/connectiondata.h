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

#ifndef _EXPORL_SRC_LIB_APEXDATA_CONNECTION_CONNECTIONDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_CONNECTION_CONNECTIONDATA_H_

#include <QString>
#include <QVector>

#include "apextools/global.h"

namespace apex
{
namespace data
{

struct ConnectionDataPrivate;

enum MatchType { MATCH_NAME, MATCH_REGEXP, MATCH_WILDCARD };

class APEXDATA_EXPORT ConnectionData
{
public:
    ConnectionData();
    ConnectionData(const ConnectionData &other);
    ~ConnectionData();

    // getters
    MatchType matchType() const;
    const QString &fromId() const;
    const QString &fromChannelId() const;
    int fromChannel() const;
    const QString &toId() const;
    const QString &toChannelId() const;
    int toChannel() const;
    QString device() const;

    // setters
    void setMatchType(MatchType t);
    void setFromId(const QString &id);
    void setFromChannel(int channel, const QString &channelId = QString());
    void setToId(const QString &id);
    void setToChannel(int channel, const QString &channelId = QString());
    void setDevice(QString id);

    ConnectionData &operator=(const ConnectionData &other);
    bool operator==(const ConnectionData &other);
    bool duplicateOf(const ConnectionData &c);

private:
    ConnectionDataPrivate *d;
};

class APEXDATA_EXPORT ConnectionsData : public QList<ConnectionData *>
{
public:
    ConnectionsData();
    ~ConnectionsData();

    bool operator==(const ConnectionsData &other);

private:
    // prevent copying
    ConnectionsData(const ConnectionsData &);
    ConnectionsData &operator=(const ConnectionsData &);
};

} // ns data
} // ns apex

#endif
