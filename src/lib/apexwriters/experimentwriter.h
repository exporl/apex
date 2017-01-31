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

#ifndef APEXWRITER_H
#define APEXWRITER_H

#include "global.h"

#include "xml/xercesinclude.h"

#include <QStringList>

namespace XERCES_CPP_NAMESPACE
{
class DOMElement;
}

namespace apex
{
namespace data
{
class ExperimentData;
}
namespace writer
{
class APEXWRITERS_EXPORT ExperimentWriter
{
    public:

        /**
         * Writes the given data to the given file.
         * If screens is not empty, the strings in the list will be parsed to
         * DOMElements and used as the screens in ScreensData. No validation
         * is used here so make sure it is valid xml.
         */
        static void write(const data::ExperimentData& data, const QString& file,
                          const QStringList& screens = QStringList());

    private:

        static void throwIfNull(XERCES_CPP_NAMESPACE::DOMElement* e,
                                const QString& failingWrite);
};
}//ns writer
}//ns apex
#endif


