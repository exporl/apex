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

#include "apexdata/datablock/datablockdata.h"

#include "stimulus/datablock.h"

#include "streamapp/file/wavefile.h"

#include "streamapp/silentreader.h"

#include "wavstimulus//streamgenerator.h"

#include "wavcompatibledatablock.h"

#include <QString>

using namespace apex;
using namespace stimulus;
using namespace streamapp;

WavCompatibleDataBlock::WavCompatibleDataBlock(
    const data::DatablockData &data, const QString &filename,
    const ExperimentRunDelegate *experiment)
    : DataBlock(data, filename, experiment)
{
}

WavCompatibleDataBlock::~WavCompatibleDataBlock()
{
}

void WavCompatibleDataBlock::SetParameter(const QString &ac_sID,
                                          const QString &ac_sVal)
{
    if (ac_sID == "loop")
        this->data.setNbLoops(ac_sVal.toUInt());
    else
        DataBlock::SetParameter(ac_sID, ac_sVal);
}
