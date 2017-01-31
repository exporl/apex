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

#ifndef __MIXERPARAMETERS_H__
#define __MIXERPARAMETERS_H__

#include "device/devicedata.h"

#include "global.h"

namespace apex
{
namespace data
{

/**
 * @enum ApexMixerType
 *
 * Supported mixers.
 */
enum MixerType
{
    DEFAULT_MIXER, //!< on Windows: MME, on Linux: OSS
    RME_MIXER,     //!< Rme HDSP mixer
    PA5_MIXER,     //!< TDT's Pa5 attenuator
    ANY_MIXER      //!< this will try to create any of the above ( tries in order listed )
};

/**
 * @class MixerParameters
 *
 * Parameters for IMixer.
 * Most functionality is in ApexDeviceParameters.
 */
class APEXDATA_EXPORT MixerParameters : public DeviceData
{
    public:

        MixerParameters();
        ~MixerParameters();

        /**
         * Get the type.
         * @return One of @ref MixerType
         */
        MixerType type() const;

        /**
         * Return type string from experiment file
         */
        QString typeString() const;

        /**
         * Get the mixer name.
         * Required by StreamappMixer.
         * @return an std::string
         */
        QString name() const;
};
}
}

#endif //#ifndef __MIXERPARAMETERS_H__
