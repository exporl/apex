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

#include "datablockdata.h"

#include "fileprefix.h"

#include <QDateTime>

namespace apex
{

namespace data
{

//struct DatablockDataPrivate

struct DatablockDataPrivate
{
    DatablockDataPrivate();
    bool operator==(const DatablockDataPrivate& other) const;

    QString    id;
    QString    file;
    FilePrefix prefix;
    QString    directData;
    QString    description;
    QString    device;
    unsigned   nbChannels;
    unsigned   nbLoops;
    double     gain;
};


DatablockDataPrivate::DatablockDataPrivate() : nbChannels(0),
                                               nbLoops(1),
                                               gain(0.0)
{
}

bool DatablockDataPrivate::operator==(const DatablockDataPrivate& other) const
{
    return
        gain == other.gain &&
        nbLoops == other.nbLoops &&
        nbChannels == other.nbChannels &&
        device == other.device &&
        description == other.description &&
        prefix == other.prefix &&
        id == other.id &&
        file == other.file &&
        directData == other.directData;
}

//class DatablockData

DatablockData::DatablockData() : d(new DatablockDataPrivate())
{
}


DatablockData::DatablockData(const DatablockData& other) :
                                         d(new DatablockDataPrivate(*other.d))
{
}

DatablockData::~DatablockData()
{
    delete d;
}

//getters

const QString& DatablockData::id() const
{
    return d->id;
}

const QString& DatablockData::device() const
{
    return d->device;
}

const QString& DatablockData::file() const
{
    return d->file;
}

const QString& DatablockData::description() const
{
    return d->description;
}

const FilePrefix& DatablockData::prefix() const
{
    return d->prefix;
}


const QString& DatablockData::directData() const
{
    return d->directData;
}


unsigned int DatablockData::nbChannels() const
{
    return d->nbChannels;
}


unsigned int DatablockData::nbLoops() const
{
    return d->nbLoops;
}


double DatablockData::gain() const
{
    return d->gain;
}

//setters

void DatablockData::setId(const QString &id)
{
    d->id = id;
}

void DatablockData::setDevice(const QString& device)
{
    d->device = device;
}

void DatablockData::setFile(const QString &file)
{
    d->file = file;
}

void DatablockData::setDescription(const QString& description)
{
    d->description = description;
}


void DatablockData::setPrefix(const FilePrefix& prefix)
{
    d->prefix = prefix;
}


void DatablockData::setDirectData(const QString& data)
{
    d->directData = data;
}


void DatablockData::setNbChannels(unsigned int nb)
{
    d->nbChannels = nb;
}


void DatablockData::setNbLoops(unsigned int nb)
{
    d->nbLoops = nb;
}


void DatablockData::setGain(double gain)
{
    d->gain = gain;
}

//operators

DatablockData& DatablockData::operator=(const DatablockData& other)
{
    if (this != &other)
        *d = *other.d;

    return *this;
}


bool DatablockData::operator==(const DatablockData& other) const
{
    return *d == *other.d;
}

} // namespace data

} // namespace apex
