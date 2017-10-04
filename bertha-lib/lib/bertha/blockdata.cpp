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

#include "blockdata.h"
#include "blockdata_private.h"

namespace bertha
{

// BlockData ===================================================================

BlockData::BlockData() : d(new BlockDataPrivate)
{
    d->inputPorts = -1;
    d->outputPorts = -1;
    d->sampleRate = 0;
    d->blockSize = 0;
}

BlockData::BlockData(const QString &id, const QString &plugin)
    : d(new BlockDataPrivate)
{
    d->inputPorts = -1;
    d->outputPorts = -1;
    d->sampleRate = 0;
    d->blockSize = 0;
    d->id = id;
    d->plugin = plugin;
}

BlockData::~BlockData()
{
}

BlockData::BlockData(const BlockData &other) : d(other.d)
{
}

BlockData &BlockData::operator=(const BlockData &other)
{
    d = other.d;
    return *this;
}

void BlockData::setId(const QString &arg)
{
    d->id = arg;
}

QString BlockData::id() const
{
    return d->id;
}

void BlockData::setPlugin(const QString &arg)
{
    d->plugin = arg;
}

QString BlockData::plugin() const
{
    return d->plugin;
}

void BlockData::setOutputPorts(int outputPorts)
{
    d->outputPorts = outputPorts;
}

int BlockData::outputPorts() const
{
    return d->outputPorts;
}

void BlockData::setInputPorts(int inputPorts)
{
    d->inputPorts = inputPorts;
}

int BlockData::inputPorts() const
{
    return d->inputPorts;
}

void BlockData::setParameter(const QString &name, const QVariant &value)
{
    const QString parameterName = name.toLower();
    if (!value.isValid())
        d->parameters.remove(parameterName);
    else
        d->parameters[parameterName] = value;
}

QVariant BlockData::parameter(const QString &name) const
{
    return d->parameters[name.toLower()];
}

QMap<QString, QVariant> BlockData::parameters() const
{
    return d->parameters;
}

}; // namespace bertha
