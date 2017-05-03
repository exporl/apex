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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULUS_H_
#define _EXPORL_SRC_LIB_APEXMAIN_STIMULUS_STIMULUS_H_

#include "apexdata/stimulus/stimulusdata.h"

#include "apextools/apextypedefs.h"

#include "streamapptypedefs.h"

namespace apex
{

class ExperimentConfigFileParser;

namespace data
{
class StimulusParameters;
}

namespace stimulus
{

class StimulusFactory;

/**
 * Stimulus
 *   represents a single stimulus.
 *   Has a list of datablocks and parameters.
 ******************************************** */
class Stimulus
{
    friend class StimulusFactory;
    friend class ExperimentConfigFileParser;
public:
    /**
     * Constructor.
     * @param ac_sID a unique identifier
     * @param ac_sDescription small description
     * @param ac_fixParams fixed parameters, cannot be changed
     * @param ac_varParams parameters that can be changed during the experiment
     */
    Stimulus( const data::StimulusData& data );

    /**
     * Destructor.
     */
    virtual ~Stimulus();

    /**
     * Get the id.
     * @return string reference
     */
    const QString GetID() const;


    /**
     * Get the playmatrix.
     * @return const pointer
     */
    PlayMatrix* GetPlayMatrix() const
    {
        return mc_PlayMatrix;
    }

    void SetPlayMatrix(PlayMatrix* pm)
    {
        mc_PlayMatrix=pm;
    }

    /**
     * Get the fixed parameters.
     * @return a pointer
     */
    const data::StimulusParameters* GetFixParameters() const;

    /**
     * Get the variable parameters.
     * It is allowed to change the parameters (eg by adaptiveprocedure).
     * @return a pointer
     */
    const data::StimulusParameters* GetVarParameters() const;

    /**
     * Get the map with datablocks ordered per device.
     * @return unmutable tQStringVectorMap
     */
    const tQStringVectorMap& GetDeviceDataBlocks() const
    {
        return m_DeviceDataBlocks;
    }


    /**
     * Get the map with datablocks ordered per device.
     * Used when a class needs to set a parameter on the devices' datablocks.
     * @return mutable tQStringVectorMap
     */
    tQStringVectorMap& ModDeviceDataBlocks()
    {
        return m_DeviceDataBlocks;
    }

    const data::StimulusData* data() const {return &m_data;}

    void ConstructDevDBlockMap( const tDeviceMap& ac_Devices, tDataBlockMap& a_DataBlocks );

private:

    const data::StimulusData m_data;
    PlayMatrix*  mc_PlayMatrix;
    tQStringVectorMap m_DeviceDataBlocks; //datablocks ordered per device
    //const QString id;
};

}
}

#endif //STIMULUS_H
