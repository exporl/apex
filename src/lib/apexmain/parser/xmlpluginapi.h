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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_PARSER_XMLPLUGINAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_PARSER_XMLPLUGINAPI_H_

#include "streamapp/file/wavefile.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

namespace apex
{

/**
 * API for XML plugins expanded by scriptexpander
 */
class XMLPluginAPI : public QObject
{
    Q_OBJECT

public slots:
    QString version();

    /**
     * Get all files in the filesystem matching wildcard
     */
    QStringList files(const QString &path);

    /**
      * Read file content into string
      */
    QString readAll(const QString &path);

    /**
      * Get the duration of a WAV file in seconds
      */
    double stimulusDuration(const QString &path);

    /**
     * Get path without filename for relative path
     */
    QString path(const QString &s);

    // Deprecated: do not use!
    QString stripPath(const QString &s);

    /**
      * Returns full filepath for relative path
      */
    QString absoluteFilePath(const QString &path);

    /**
     * Add warning message to message window
     */
    void addWarning(const QString &warning);

    /**
     * Add error message to message window
     */
    void addError(const QString &warning);
};
}

#endif
