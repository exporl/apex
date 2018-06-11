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

#include <QString>
#include <iostream>

#include "dummydevice.h"
#include "stimulus/datablock.h"
#include "stimulus/filter.h"

namespace apex
{
namespace stimulus
{

DummyDevice::DummyDevice(data::DeviceData &a_Params) : OutputDevice(&a_Params)
{
}

DummyDevice::~DummyDevice()
{
}

bool DummyDevice::HasError() const
{
    Say(QString("HasError = false"));
    return false;
}

const QString &DummyDevice::GetID() const
{
    Say(QString("GetID") + OutputDevice::GetID());
    return OutputDevice::GetID();
}

bool DummyDevice::AllDone()
{
    Say("AllDone");
    return true;
}

void DummyDevice::AddFilter(Filter &ac_Filter)
{
    Say("AddFilter: " + ac_Filter.GetID());
    Say("warning potential mem leak!");
}

void DummyDevice::AddInput(const DataBlock &ac_DataBlock)
{
    Say("AddInput: " + ac_DataBlock.GetID());
}

void DummyDevice::PlayAll()
{
    Say("PlayAll");
}

void DummyDevice::RemoveAll()
{
    Say("RemoveAll");
}

void DummyDevice::StopAll()
{
    Say("StopAll");
}

void DummyDevice::Say(const QString &message) const
{
    std::cout << "DummyDevice: " << message.toLocal8Bit().data() << std::endl;
}

void DummyDevice::SetSequence(const apex::stimulus::DataBlockMatrix *p)
{
    OutputDevice::SetSequence(p); // delete it
    Say("SetSequence");
}

bool DummyDevice::SetParameter(const QString &type, const int,
                               const QVariant &value)
{
    Say("SetParameter " + type + " to " + value.toString());
    return true;
}

void DummyDevice::RestoreParameters()
{
    Say("RestoreParameters");
}
}
}
