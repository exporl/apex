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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_DUMMYSTIMULUS_DUMMYDEVICE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_DUMMYSTIMULUS_DUMMYDEVICE_H_

/*
Dummy device, only prints debugging output

*/

#include "apextools/xml/xmlkeys.h"

#include "stimulus/outputdevice.h"

#include "streamapptypedefs.h"

using namespace apex::XMLKeys;

namespace apex
{
namespace stimulus
{

/**
@author Tom Francart,,,
*/
class DummyDevice : public OutputDevice
{
public:
    DummyDevice(data::DeviceData &a_Params);

    ~DummyDevice();

    virtual bool AllDone();
    virtual void AddFilter(Filter &ac_Filter);
    virtual void AddInput(const DataBlock &ac_DataBlock);
    virtual void PlayAll();
    virtual void RemoveAll();
    virtual void StopAll();
    virtual bool SetParameter(const QString &type, int channel,
                              const QVariant &value);

    /**
     * Reset the filter to its initial state and set all internal parameters to
     * built in
     * default values
     * throw exception if problem
     */
    virtual void Reset(){};

    /**
     * Prepare filter for processing
     * throw exception if problem
     */
    virtual void Prepare(){};

    virtual bool HasError() const;
    virtual const QString &GetID() const;

    virtual bool CanSequence() const
    {
        return true;
    }

    virtual void SetSequence(const apex::stimulus::DataBlockMatrix *);

    virtual void RestoreParameters();

    virtual void SetSilenceBefore(double)
    {
        qCDebug(APEX_RS, "DummyDevice: SetSilenceBefore not implemented");
    };

private:
    void Say(const QString &message) const;
};
}
}

#endif
