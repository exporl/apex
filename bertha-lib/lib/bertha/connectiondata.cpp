/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with this program; if not, write to the Free Software Foundation, Inc.,    *
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                *
 ******************************************************************************/

#include "connectiondata.h"

namespace bertha
{

class ConnectionDataPrivate : public QSharedData
{
public:
    QString sourceBlock;
    QString sourcePort;
    QString targetBlock;
    QString targetPort;
};

// ConnectionData ==============================================================

ConnectionData::ConnectionData() : d(new ConnectionDataPrivate)
{
}

ConnectionData::ConnectionData(const QString &sourceBlock,
                               const QString &sourcePort,
                               const QString &targetBlock,
                               const QString &targetPort)
    : d(new ConnectionDataPrivate)
{
    d->sourceBlock = sourceBlock;
    d->sourcePort = sourcePort;
    d->targetBlock = targetBlock;
    d->targetPort = targetPort;
}

ConnectionData::~ConnectionData()
{
}

ConnectionData::ConnectionData(const ConnectionData &other) : d(other.d)
{
}

ConnectionData &ConnectionData::operator=(const ConnectionData &other)
{
    d = other.d;
    return *this;
}

QString ConnectionData::sourceBlock() const
{
    return d->sourceBlock;
}

QString ConnectionData::sourcePort() const
{
    return d->sourcePort;
}

QString ConnectionData::targetBlock() const
{
    return d->targetBlock;
}

QString ConnectionData::targetPort() const
{
    return d->targetPort;
}

void ConnectionData::setSourceBlock(const QString &block)
{
    d->sourceBlock = block;
}

void ConnectionData::setTargetBlock(const QString &block)
{
    d->targetBlock = block;
}

void ConnectionData::setSourcePort(const QString &port)
{
    d->sourcePort = port;
}

void ConnectionData::setTargetPort(const QString &port)
{
    d->targetPort = port;
}

}; // namespace bertha
