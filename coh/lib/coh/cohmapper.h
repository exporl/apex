/******************************************************************************
 * Copyright (C) 2016  Michael Hofmann <mh21@mh21.de>                         *
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

#ifndef _EXPORL_COH_LIB_COH_COHMAPPER_H_
#define _EXPORL_COH_LIB_COH_COHMAPPER_H_

#include "common/global.h"

#include "coh.h"

namespace coh
{

class CohSequenceMapperPrivate;

class COH_EXPORT CohSequenceMapper
{
public:
    CohSequenceMapper(CohSequence *sequence);
    virtual ~CohSequenceMapper();

    void setDefaultReferenceElectrode(Coh::Electrode reference);
    void setDefaultPhaseWidth(double width);
    void setDefaultPhaseGap(double gap);
    void setDefaultPeriod(double interval);
    // volume and magnitude between 0 and 1
    void setChannel(int channel, Coh::Electrode active,
                    Coh::Electrode reference, unsigned t, unsigned c,
                    double volume);

    Coh::CommandProperties needsMapping();
    CohSequence *map();

private:
    DECLARE_PRIVATE(CohSequenceMapper)
protected:
    DECLARE_PRIVATE_DATA(CohSequenceMapper)
};

} // namespace coh

#endif
