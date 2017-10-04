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

#include "wavgenerator.h"

#include <QString>

namespace apex
{

namespace data
{
class FilterData;
}

namespace stimulus
{

class DataLoopGeneratorFilter : public WavGenerator
{
public:
    DataLoopGeneratorFilter(const QString &ac_sID, const QString &ac_sType,
                            data::FilterData *pParams, unsigned long sr,
                            unsigned bs)
        : WavGenerator(ac_sID, ac_sType, pParams, sr, bs)
    {
    }

    void SetSource(stimulus::DataBlock *s)
    {
        m_src = s;
    };
    stimulus::DataBlock *GetSource() const
    {
        Q_CHECK_PTR(m_src);
        return m_src;
    };

private:
    stimulus::DataBlock *m_src;
};

} // ns stimulus
} // ns apex
