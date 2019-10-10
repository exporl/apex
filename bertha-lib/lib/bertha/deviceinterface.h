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

#ifndef _BERTHA_SRC_LIB_BERTHA_DEVICEINTERFACE_H_
#define _BERTHA_SRC_LIB_BERTHA_DEVICEINTERFACE_H_

#include "blockinterface.h"
#include "global.h"

#include <QMap>
#include <QStringList>
#include <QVariant>

#include <QtPlugin>

/**
 * This class is a base class for sound drivers
*/
class BERTHA_EXPORT DeviceInterface : public BasicBlockInterface
{
public:
    typedef void (*Callback)(void *);

    /** Virtual destructor to make the compiler happy. */
    virtual ~DeviceInterface();

    /** The default nomenclature of ports is
    *   capture-1, capture-2,
    *   playback-1, playback-2
    */
    // virtual QList<BlockOutputPort *> outputPorts() = 0;
    // virtual QList<BlockInputPort *> inputPorts() = 0;

    virtual void setCallback(Callback callback, void *data) = 0;

    // returns a non-empty string if sth. goes wrong
    virtual QString prepare() = 0;
    // returns a non-empty string if sth. goes wrong
    virtual QString start() = 0;
    virtual void stop() = 0;
    virtual void release() = 0;

    // return false if the processing should stop
    virtual bool processOutput() = 0;
    // return false if the processing should stop
    virtual bool processInput() = 0;
};

class DeviceCreatorInterface
{
public:
    virtual ~DeviceCreatorInterface()
    {
    }

    virtual QList<BlockDescription> availableBlocks() const = 0;
};

Q_DECLARE_INTERFACE(DeviceCreatorInterface,
                    "be.exporl.bertha.devicecreator/1.0")
Q_DECLARE_INTERFACE(DeviceInterface, "be.exporl.bertha.device/1.0")

#endif
