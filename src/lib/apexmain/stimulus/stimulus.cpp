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

#include "apexdata/stimulus/stimulusparameters.h"

#include "datablock.h"
#include "playmatrix.h"
#include "stimulus.h"

#include <QtGlobal>

using namespace apex;
using namespace apex::stimulus;

Stimulus::Stimulus(const data::StimulusData &data) : m_data(data)
{
    PlayMatrixCreator creator;
    mc_PlayMatrix = creator.createMatrix(data.GetDatablocksContainer());

    // PlayMatrixCreator::sf_DoDisplay( mc_PlayMatrix );
}

Stimulus::~Stimulus()
{
    delete mc_PlayMatrix; // this is not a FactoryElement so delete it
}

/**
  * Get the fixed parameters.
  * @return a pointer
  */
const StimulusParameters *Stimulus::GetFixParameters() const
{
    return &m_data.GetFixedParameters();
}

const QString Stimulus::GetID() const
{
    return m_data.GetID();
}

/**
  * Get the variable parameters.
  * It is allowed to change the parameters (eg by adaptiveprocedure).
  * @return a pointer
  */
const StimulusParameters *Stimulus::GetVarParameters() const
{
    return &m_data.GetVariableParameters();
}

void Stimulus::ConstructDevDBlockMap(const tDeviceMap &ac_Devices,
                                     tDataBlockMap &a_DataBlocks)
{
    const PlayMatrix &ac_Mat = *mc_PlayMatrix;
    const unsigned nS = ac_Mat.mf_nGetBufferSize();
    const unsigned nP = ac_Mat.mf_nGetChannelCount();
    //    Q_ASSERT( nS && nP );
    tQStringVectorMap &dmap = m_DeviceDataBlocks;

    for (tDeviceMapCIt it = ac_Devices.begin(); it != ac_Devices.end(); ++it)
        dmap[it.key()] = tQStringVector();

    for (unsigned i = 0; i < nS; ++i) {
        for (unsigned j = 0; j < nP; ++j) {
            const QString &sCur = ac_Mat(j, i);
            if (!sCur.isEmpty()) {
                DataBlock *pCur = a_DataBlocks.value(sCur);
                Q_ASSERT(pCur);
                dmap[pCur->GetDevice()].push_back(sCur);
            }
        }
    }
}
