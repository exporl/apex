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

#ifndef _APEX_SRC_LIB_APEXWRITERS_EXPERIMENTWRITER_H_
#define _APEX_SRC_LIB_APEXWRITERS_EXPERIMENTWRITER_H_

#include "apextools/global.h"

#include <QStringList>

class QDomElement;

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
    Q_DECLARE_TR_FUNCTIONS(ExperimentWriter)
public:
    /**
     * Writes the given data to the given file.
     * If screens is not empty, the strings in the list will be parsed to
     * DOMElements and used as the screens in ScreensData. No validation
     * is used here so make sure it is valid xml.
     */
    static void write(const data::ExperimentData &data, const QString &file,
                      const QStringList &screens = QStringList());

private:
    static void throwIfNull(const QDomElement &e, const QString &where);
};
}
}

#endif
