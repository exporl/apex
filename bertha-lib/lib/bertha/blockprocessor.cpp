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

#include "common/exception.h"
#include "common/pluginloader.h"

#include "blockdata.h"
#include "blockinterface.h"
#include "blockprocessor.h"

using namespace cmn;

namespace bertha
{

// BlockProcessor ==============================================================

BlockProcessor::BlockProcessor(const BlockData &data, unsigned blockSize,
                               unsigned sampleRate)
    : AbstractProcessor(data, blockSize, sampleRate, availableBlockPlugins())
{
    block = castPlugin<BlockInterface>();
}

BlockProcessor::~BlockProcessor()
{
}

void BlockProcessor::prepare(unsigned numberOfFrames)
{
    const QString errorMessage = block->prepare(numberOfFrames);
    if (!errorMessage.isEmpty())
        throw Exception(
            tr("Unable to prepare block %1: %2").arg(id(), errorMessage));
}

void BlockProcessor::process()
{
    block->process();
}

void BlockProcessor::release()
{
    block->release();
}

QMap<QString, BlockDescription> BlockProcessor::availableBlockPlugins()
{
    return availableCreatorPlugins<BlockCreatorInterface>();
}

BlockInterface *BlockProcessor::blockInterface() const
{
    return block;
}

}; // namespace bertha
