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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_COHSTIMULUS_COHDEVICE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_COHSTIMULUS_COHDEVICE_H_

#include "stimulus/outputdevice.h"

#include <QString>

namespace apex
{

namespace data
{
class CohDeviceData;
}

namespace stimulus
{

class CohDevicePrivate;

class CohDevice : public OutputDevice
{
    Q_DECLARE_TR_FUNCTIONS(CohDevice)
public:
    CohDevice(data::CohDeviceData *params);
    ~CohDevice();

    virtual void AddFilter(Filter &filter) Q_DECL_OVERRIDE;
    virtual void AddInput(const DataBlock &dataBlock) Q_DECL_OVERRIDE;
    virtual void SetSequence(const DataBlockMatrix *sequence) Q_DECL_OVERRIDE;
    virtual void RemoveAll() Q_DECL_OVERRIDE;
    virtual void PlayAll() Q_DECL_OVERRIDE;
    virtual void StopAll() Q_DECL_OVERRIDE;
    virtual bool AllDone() Q_DECL_OVERRIDE;
    virtual bool SetParameter(const QString &type, int channel, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void Reset() Q_DECL_OVERRIDE;
    virtual void Prepare() Q_DECL_OVERRIDE;
    virtual void SetSilenceBefore(double time) Q_DECL_OVERRIDE;
    virtual bool CanSequence() const Q_DECL_OVERRIDE;

private:
    DECLARE_PRIVATE(CohDevice)

protected:
    DECLARE_PRIVATE_DATA(CohDevice)
};

}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_COHSTIMULUS_COHDEVICE_H_
