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

#ifndef CALIBRATIONWRITER_H
#define CALIBRATIONWRITER_H

#include "global.h"

#include "xml/xercesinclude.h"

#include <QMap>

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
class CalibrationData;
class SoundLevelMeterData;
class CalibrationParameterData;
}

namespace writer
{
using namespace XERCES_CPP_NAMESPACE;

/**
*@author Job Noorman <jobnoorman@gmail.com>
*/
class APEXWRITERS_EXPORT CalibrationWriter
{
    public:
        /**
        *Adds a <result> element to the document.
        *
        *@param doc  the document to place the element in
        *@param data the data to create the element with
        *
        *@return the created element
         */
        static DOMElement* addElement(DOMDocument* doc,
                                      const data::CalibrationData& data);

    private:

        static DOMElement* addSoundLevelMeter(DOMDocument* doc,
                                       const data::SoundLevelMeterData& data);
        static DOMElement* addParameters(DOMDocument* doc,
                     const QMap<QString, data::CalibrationParameterData>& data);
        static void appendParameterTo(DOMElement* slm, DOMDocument* doc,
                          QString param, const data::SoundLevelMeterData& data);
        static DOMElement* addStimuli(DOMDocument* doc, const QStringList& data);
};

}

}

#endif
