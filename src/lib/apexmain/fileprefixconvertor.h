/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#ifndef _APEX_SRC_LIB_APEXMAIN_FILEPREFIXCONVERTOR_H_
#define _APEX_SRC_LIB_APEXMAIN_FILEPREFIXCONVERTOR_H_

#include "apexdata/fileprefix.h"

#include <QCoreApplication>

namespace apex
{

/**
 * Converts a FilePrefix into a real path name
 */
class FilePrefixConvertor
{
    Q_DECLARE_TR_FUNCTIONS(FilePrefixConvertor);

public:
    static QString convert(data::FilePrefix p);
    static QString addPrefix(data::FilePrefix p, const QString &file);
};
}

#endif
