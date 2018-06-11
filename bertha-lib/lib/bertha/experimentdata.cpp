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

#include "common/exception.h"

#include "experimentdata.h"

using namespace cmn;

namespace bertha
{

class ExperimentDataPrivate : public QSharedData
{
public:
    DeviceData device;
    QList<BlockData> blocks;
    QList<ConnectionData> connections;
    QString documentDirectory;
};

// ExperimentData ==============================================================

ExperimentData::ExperimentData() : d(new ExperimentDataPrivate)
{
}

ExperimentData::~ExperimentData()
{
}

ExperimentData::ExperimentData(const ExperimentData &other) : d(other.d)
{
}

ExperimentData &ExperimentData::operator=(const ExperimentData &other)
{
    d = other.d;
    return *this;
}

bool ExperimentData::blockExists(const QString &id) const
{
    Q_FOREACH (const BlockData &block, d->blocks)
        if (block.id() == id)
            return true;
    return false;
}

void ExperimentData::addBlock(const BlockData &block)
{
    const QString id = block.id();
    QMutableListIterator<BlockData> i(d->blocks);
    while (i.hasNext()) {
        i.next();
        if (i.value().id() == id)
            i.remove();
    }
    d->blocks.append(block);
}

void ExperimentData::removeBlockAt(int index)
{
    d->blocks.removeAt(index);
}

BlockData ExperimentData::block(const QString &id) const
{
    Q_FOREACH (const BlockData &block, d->blocks)
        if (block.id() == id)
            return block;
    throw Exception(tr("Block not found: %1").arg(id));
}

QList<BlockData> ExperimentData::blocks() const
{
    return d->blocks;
}

void ExperimentData::addConnection(const ConnectionData &connection)
{
    d->connections.push_back(connection);
}

void ExperimentData::removeConnectionAt(int index)
{
    d->connections.removeAt(index);
}

void ExperimentData::setDevice(const DeviceData &device)
{
    d->device = device;
}

QList<ConnectionData> ExperimentData::connections() const
{
    return d->connections;
}

DeviceData ExperimentData::device() const
{
    return d->device;
}

QString ExperimentData::getDocumentDirectory() const
{
    return d->documentDirectory;
}

void ExperimentData::setDocumentDirectory(QString value)
{
    d->documentDirectory = value;
}

}; // namespace bertha
