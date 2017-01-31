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
 
#ifndef WAVCOMPATIBLEDATABLOCK_H
#define WAVCOMPATIBLEDATABLOCK_H

#include "../datablock.h"

namespace streamapp
{
  class PositionableAudioFormatReaderStream;
}

namespace apex
{

class ExperimentRunDelegate;

namespace data
{
class DatablockData;
}
namespace stimulus
{

class WavCompatibleDataBlock : public DataBlock
{
public:
    /**
        * Constructor.
        */
    WavCompatibleDataBlock(const data::DatablockData& data,
                           const QUrl& filename,
                           const ExperimentRunDelegate* experiment);

    /**
        * Destructor.
        */
    virtual ~WavCompatibleDataBlock();

    /**
        * Get the PositionableAudioFormatReaderStream
        * for this WavDataBlock.
        * @param ac_nBufferSize the buffersize
        * @param ac_lFs the samplerate, used to calculate silence length
        * @return new'd pointer or 0 for error
        */
    virtual streamapp::PositionableAudioFormatReaderStream* GetWavStream( const unsigned ac_nBufferSize, const unsigned long ac_nFs ) const = 0;

    /**
        * Implementation of the DataBlock method.
        * Sets number of loops if ac_sName == "loop",
        * else calls DataBlock::SetParameter()
        */
    virtual void SetParameter( const QString& ac_sName, const QString& ac_sVal );

    /**
        * Implementation of the DataBlock method.
        */
    virtual const QString& GetModule() const
    { return sc_sWavDevice; }
};

}
}
#endif //WAVDATABLOCK_H

