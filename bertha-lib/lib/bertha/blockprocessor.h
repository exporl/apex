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

#ifndef _BERTHA_SRC_LIB_BERTHA_BLOCKPROCESSOR_H_
#define _BERTHA_SRC_LIB_BERTHA_BLOCKPROCESSOR_H_

#include "abstractprocessor.h"

namespace bertha
{

class BlockData;

class BlockProcessorPrivate;

class BERTHA_EXPORT BlockProcessor : public AbstractProcessor
{
public:
    BlockProcessor(const BlockData &data, unsigned blockSize,
                   unsigned sampleRate);
    virtual ~BlockProcessor();

    /**
     * Prepares the blockprocessor.
     * @param numberOfFrames The number of frames that will be parsed.
     */
    void prepare(unsigned numberOfFrames);
    void process();
    void release();

    /**
      * Return internal pointer to BlockInterface
      * this should normally not be used, except e.g. in a test-bench
      */
    BlockInterface *blockInterface() const;

    static QMap<QString, BlockDescription> availableBlockPlugins();

private:
    BlockInterface *block;
};

}; // namespace bertha

#endif
