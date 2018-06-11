/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _APEX_SRC_LIB_APEXWRITERS_STIMULIWRITER_H_
#define _APEX_SRC_LIB_APEXWRITERS_STIMULIWRITER_H_

#include "apextools/global.h"

class QStringList;

namespace apex
{
namespace data
{
class StimuliData;
class StimulusData;
class StimulusParameters;
struct StimulusDatablocksContainer;
}

namespace writer
{

/**
 * @author Job Noorman <jobnoorman@gmail.com>
 */
class APEXWRITERS_EXPORT StimuliWriter
{
public:
    static QDomElement addElement(QDomDocument *doc,
                                  const data::StimuliData &data);

private:
    static QDomElement addFixedParameters(QDomDocument *doc,
                                          const QStringList &params);
    static QDomElement addStimulus(QDomDocument *doc,
                                   const data::StimulusData &data);
    static QDomElement
    addDatablocks(QDomDocument *doc,
                  const data::StimulusDatablocksContainer &data);
    static QDomElement addParameters(QDomDocument *doc,
                                     const data::StimulusParameters &params,
                                     const QString &name);
};
}
}

#endif
