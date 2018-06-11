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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_IAPEXDEVICE_H_
#define _EXPORL_SRC_LIB_APEXMAIN_DEVICE_IAPEXDEVICE_H_

#include "apexdata/device/devicedata.h"

#include "parameters/parametermanager.h"

#include "apexcontrol.h"

#include <QDebug>
#include <QString>

namespace apex
{

namespace data
{
class DeviceData;
}

namespace device
{

/**
  * IApexDevice
  *   interface for all hardware devices.
  *   Actually nothing but id+parameters.
  *************************************** */
class IApexDevice
{
protected:
    /**
     * Constructor.
     * @param ac_sID a unique ID
     * @param a_pParameters ptr to the initial (aka default) parameters, must be
     * valid, won't be deleted
     */
    IApexDevice(data::DeviceData *a_pParameters);

public:
    /**
     * Destructor.
     */
    virtual ~IApexDevice();

    /**
     * Get the ID
     * @return ID
     */
    virtual const QString &GetID() const;

    /**
     * Checks if a parameter of the given type exists.
     * @param type the type of parameter
     * @return true if a parameter was found
     */
    virtual bool HasParameter(const QString &type) const;

    /**
     * Set parameters by querying the given parametermanager object
     */
    virtual void SetParameters(const ParameterManager &pm);
    virtual bool SetParameter(const QString &type, int channel,
                              const QVariant &value) = 0;
    virtual bool SetParameter(const data::Parameter &name,
                              const QVariant &value);

    /**
     * Restore all parameters to their default (as specified in the config)
     * value.
     */
    virtual void RestoreParameters();

    /**
     * Reset the device to its initial state and set all internal parameters to
     * built in
     * default values
     * throw exception if problem
     */
    virtual void Reset() = 0;

    /**
     * Prepare device for sending output
     * throw exception if problem
     */
    virtual void Prepare() = 0;

    virtual void release(){};

    /**
     * Get all parameters.
     */
    virtual const data::DeviceData &GetParameters() const;

    /**
     * Get Parameters.
     * @return mutable parameters, as required by some devices
     */
    virtual data::DeviceData *ModParameters() const;

    /**
     * Get start of xml description of what has been going on.
     * @return result string
     */
    virtual QString GetResultXML() const;

    /**
     * Get end of xml description.
     * @return result string
     */
    virtual QString GetEndXML() const;

    /**
    * Get device specific info.
    * generic method for later use.
    * @param type the type, normally an enum in subclasses
    * @param info pointer to data to fill with info
    * @return true if a_pInfo contains new info
    */
    virtual bool GetInfo(const unsigned /*type*/, void * /*info*/) const;

    /**
    * mt_eInfoType
    *   specifies info types most devices can handle.
    *   Devices using other types must use them from the range above mc_eCustom.
    ****************************************************************************
    */
    enum mt_eInfoType {
        mc_eClipping,   //!< query clipping, uses tUnsignedBoolMapCIt* for the
                        //! void* pointer
        mc_eOutOfRange, //!< query if the last parameter that was set was in
                        //! range, use QString* for the pointer
        mc_eCustom = 1000
    };

private:
    const QString mc_sID2;

protected:
    data::DeviceData *m_pParameters;
    bool mv_bNeedsRestore;
};
}
}

#endif //#ifndef _EXPORL_SRC_LIB_APEXMAIN_DEVICE_IAPEXDEVICE_H_
