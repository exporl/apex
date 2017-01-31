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

#ifndef __CLARIONDATABLOCK_H__
#define __CLARIONDATABLOCK_H__

#include "stimulus/datablock.h"

namespace apex
{

class ExperimentRunDelegate;

namespace data
{
class DatablockData;
}

namespace stimulus
{

class ClarionDataBlock : public DataBlock
{
public:
ClarionDataBlock(const data::DatablockData& data, const QUrl& filename,
                  const ExperimentRunDelegate* experiment);
    ~ClarionDataBlock();


    const QString& GetModule() const
    { return sc_sClarionDevice; }


};

}
}

#endif //#ifndef __CLARIONDATABLOCK_H__
