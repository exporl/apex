/******************************************************************************
 * Copyright (C) 2008  Michael Hofmann <mh21@piware.de>                       *
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

#ifndef _EXPORL_COMMON_LIB_COMMON_PATHS_H_
#define _EXPORL_COMMON_LIB_COMMON_PATHS_H_

#include "global.h"

#include <QStringList>

namespace cmn
{

class COMMON_EXPORT Paths
{
public:
    static QStringList pluginDirectories();
    static QStringList dataDirectories();
    static QStringList binDirectories();
    static QStringList iconDirectories();

    // directory for new data files
    static QString dataDirectory();

    // empty if not installed
    static QString installedBaseDirectory();
    // android files directory
    static QString filesDirectory();

    // returns the manual temp directory if it is non-empty and if it exists,
    // otherwise the system temp dir
    static QString tempDirectory();
    static QString manualTempDirectory();
    static void setManualTempDirectory(const QString directory);

    static QString searchFile(const QString &filePath,
                              const QStringList &directories);
    static QString searchDirectory(const QString &filePath,
                                   const QStringList &directories);
};

} // namespace cmn

#endif
