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

#ifndef _EXPORL_SRC_LIB_APEXDATA_DATABLOCK_DATABLOCKDATA_H_
#define _EXPORL_SRC_LIB_APEXDATA_DATABLOCK_DATABLOCKDATA_H_

#include "apextools/global.h"

class QDateTime;

namespace apex
{

namespace data
{

struct DatablockDataPrivate;
class FilePrefix;

/**
 * Data structure for all datablocks
 */
class APEXDATA_EXPORT DatablockData
{
public:
    DatablockData();
    DatablockData(const DatablockData &other);
    virtual ~DatablockData();

    // getters
    const QString &id() const;
    const QString &device() const;
    const QString &file() const;
    const QString &description() const;
    const FilePrefix &prefix() const;
    const QString &directData() const;
    unsigned nbChannels() const;
    unsigned nbLoops() const;
    double gain() const;

    // setters
    void setId(const QString &id);
    void setDevice(const QString &device);
    void setFile(const QString &url);
    void setDescription(const QString &description);
    void setPrefix(const FilePrefix &prefix);
    void setDirectData(const QString &data);
    void setNbChannels(unsigned nb);
    void setNbLoops(unsigned nb);
    void setGain(double gain);

    // operators
    DatablockData &operator=(const DatablockData &other);
    bool operator==(const DatablockData &other) const;

private:
    DatablockDataPrivate *d;
};

} // namespace data

} // namespace apex

#endif
