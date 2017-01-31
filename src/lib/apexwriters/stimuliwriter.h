/******************************************************************************
 * Copyright (C) 2008  Job Noorman <jobnoorman@gmail.com>                     *
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

#ifndef _EXPORL_SRC_LIB_APEXWRITERS_STIMULIWRITER_H_
#define _EXPORL_SRC_LIB_APEXWRITERS_STIMULIWRITER_H_

#include "apextools/global.h"

#include "apextools/xml/xercesinclude.h"

class QStringList;

namespace XERCES_CPP_NAMESPACE
{
class DOMDocument;
class DOMElement;
}

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

        static XERCES_CPP_NAMESPACE::DOMElement *addElement(XERCES_CPP_NAMESPACE::DOMDocument *doc, const data::StimuliData &data);

    private:

        static XERCES_CPP_NAMESPACE::DOMElement *addFixedParameters(XERCES_CPP_NAMESPACE::DOMDocument *doc, const QStringList& params);
        static XERCES_CPP_NAMESPACE::DOMElement *addStimulus(XERCES_CPP_NAMESPACE::DOMDocument *doc, const data::StimulusData& data);
        static XERCES_CPP_NAMESPACE::DOMElement *addDatablocks(XERCES_CPP_NAMESPACE::DOMDocument *doc,
                                  const data::StimulusDatablocksContainer& data);
        static XERCES_CPP_NAMESPACE::DOMElement *addParameters(XERCES_CPP_NAMESPACE::DOMDocument *doc,
                                  const data::StimulusParameters& params,
                                  const QString& name);
};

}// ns writer

}// ns apex

#endif
