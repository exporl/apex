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

#ifndef SCREENSWRITER_H
#define SCREENSWRITER_H

#include "global.h"

#include "xml/xercesinclude.h"

#include <QStringList>

namespace XERCES_CPP_NAMESPACE
{
class DOMDocument;
class DOMElement;
}

namespace apex
{
namespace data
{
class Screen;
class ScreenElement;
class ButtonGroup;
class ScreensData;
}


namespace writer
{
using namespace XERCES_CPP_NAMESPACE;

/**
 * @author Job Noorman <jobnoorman@gmail.com>
 */
class APEXWRITERS_EXPORT ScreensWriter
{
    public:

        /**
         * If the given stringlist is not empty, this method will extract
         * general data (like reinforcement) from the ScreensData but the
         * screens themselves will be extracted from the QStringList. Every
         * string in this list must be an xml tree.
         * Otherwise, everything will be extracted from the data.
         *
         * @note    No validations is done on the given strings so you better
         *          be sure they are valid!
         */
        static DOMElement *addElement(DOMDocument *doc,
                                      const data::ScreensData &data,
                                      const QStringList& screens = QStringList());
        static DOMElement *addScreen(DOMDocument *doc, const data::Screen &data);

    private:

        static DOMElement *screenElementToXml(DOMDocument *doc,
                                       const data::ScreenElement &e);
        static DOMElement *buttonGroupToXml(DOMDocument *doc,
                                     const data::ButtonGroup &g);
        static DOMElement *addReinforcement(DOMDocument *doc,
                                     const data::ScreensData &data);
        static DOMElement* addGeneral(DOMDocument *doc,
                              const data::ScreensData &data);
        static DOMElement* addChildmode(DOMDocument *doc,
                               const data::ScreensData &data);
};

}// ns writer

}// ns apex

#endif
