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

#ifndef _BERTHA_SRC_LIB_BERTHA_CONNECTIONDATA_H_
#define _BERTHA_SRC_LIB_BERTHA_CONNECTIONDATA_H_

#include "global.h"

#include <QSharedDataPointer>
#include <QString>

namespace bertha
{

class ConnectionDataPrivate;

/**
 * This class represents a connection.
*/
class BERTHA_EXPORT ConnectionData
{
public:
    /**
    * Constructor: creates a default connection.
    */
    ConnectionData();

    ConnectionData(const QString &sourceBlock, const QString &sourcePort,
                   const QString &targetBlock, const QString &targetPort);

    /**
    * The virtual destructor
    */
    virtual ~ConnectionData();

    ConnectionData(const ConnectionData &other);
    ConnectionData &operator=(const ConnectionData &other);

    /**
    * Returns the id source the block where the connections leaves.
    */
    QString sourceBlock() const;

    /**
    * Returns the incomming port.
    */
    QString sourcePort() const;

    /**
    * Returns the id source the block where the connections arrives.
    */
    QString targetBlock() const;

    /**
    * Returns the outcomming port.
    */
    QString targetPort() const;

    /**
    * @param id The id of the block where the connection leaves.
    */
    void setSourceBlock(const QString &block);

    /**
    * @param port The incomming port.
    */
    void setSourcePort(const QString &port);

    /**
    * @param id The id of the block where the connection arrives.
    */
    void setTargetBlock(const QString &id);

    /**
    * @param port The outcomming port.
    */
    void setTargetPort(const QString &port);

private:
    QSharedDataPointer<ConnectionDataPrivate> d;
};

}; // namespace bertha

#endif
