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

#ifndef _BERTHA_SRC_LIB_BERTHA_DEVICEDATA_H_
#define _BERTHA_SRC_LIB_BERTHA_DEVICEDATA_H_

#include "blockdata.h"
#include "global.h"

#include <QSharedDataPointer>
#include <QString>

namespace bertha
{

class DeviceDataPrivate;

/**
 * This class represents a device.
*/
class BERTHA_EXPORT DeviceData : public BlockData
{
public:
    /**
    * Constructor: default
    */
    DeviceData();

    DeviceData(const QString &id, const QString &plugin);

    DeviceData(const BlockData &blockData);

    /**
    * Virtual destructor
    */
    virtual ~DeviceData();

    /**
    * @param sampleRate The sample rate of the device.
    */
    void setSampleRate(unsigned sampleRate);

    /**
    * Returns the samplerate used by the device.
    */
    unsigned sampleRate() const;

    /**
    * @param blockSize The blocksize of the device.
    */
    void setBlockSize(unsigned blockSize);

    /**
    * Returns the blocksize used by the device.
    */
    unsigned blockSize() const;
};

}; // namespace bertha

#endif
