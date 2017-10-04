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

#ifndef _BERTHA_SRC_LIB_BERTHA_EXPERIMENTDATA_H_
#define _BERTHA_SRC_LIB_BERTHA_EXPERIMENTDATA_H_

#include "blockdata.h"
#include "connectiondata.h"
#include "devicedata.h"
#include "global.h"

#include <QCoreApplication>
#include <QList>
#include <QMap>

#include <QSet>

namespace bertha
{

class ExperimentDataPrivate;

/**
 * This class represents an experiment. Implicitly shared.
*/
class BERTHA_EXPORT ExperimentData
{
    Q_DECLARE_TR_FUNCTIONS(ExperimentData)
public:
    /**
    * Constructor: default
    */
    ExperimentData();

    /**
    * Virtual destructor
    */
    virtual ~ExperimentData();

    ExperimentData(const ExperimentData &other);
    ExperimentData &operator=(const ExperimentData &other);

    void setDevice(const DeviceData &device);
    DeviceData device() const;

    void addBlock(const BlockData &block);
    void removeBlockAt(int index);
    // throws if not found
    BlockData block(const QString &id) const;
    QList<BlockData> blocks() const;

    void addConnection(const ConnectionData &connection);
    void removeConnectionAt(int index);
    QList<ConnectionData> connections() const;

    QString getDocumentDirectory() const;
    void setDocumentDirectory(QString value);

private:
    QSharedDataPointer<ExperimentDataPrivate> d;
};

}; // namespace bertha

#endif
