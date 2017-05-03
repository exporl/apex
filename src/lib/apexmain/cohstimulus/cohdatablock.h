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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_COHSTIMULUS_COHDATABLOCK_H_
#define _EXPORL_SRC_LIB_APEXMAIN_COHSTIMULUS_COHDATABLOCK_H_

#include "../stimulus/datablock.h"

#include "common/global.h"

#include <QCoreApplication>
#include <QSharedPointer>
#include <QStringList>
#include <QVector>

namespace coh
{
class CohSequence;
}

namespace apex
{

class ExperimentRunDelegate;

namespace data
{
class ApexMap;
}

namespace stimulus
{

class CohDataBlockPrivate;

class APEX_EXPORT CohDataBlock : public DataBlock
{
    Q_DECLARE_TR_FUNCTIONS(CohDataBlock)
public:
    CohDataBlock(const data::DatablockData& data, const QString& fileName,
                 const ExperimentRunDelegate* experiment);
    virtual ~CohDataBlock();

    coh::CohSequence* mappedData(data::ApexMap* pMap, float volume) const;

private:
    DECLARE_PRIVATE(CohDataBlock)

protected:
    DECLARE_PRIVATE_DATA(CohDataBlock)
};

}
}

#endif
