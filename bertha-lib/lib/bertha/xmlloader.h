/******************************************************************************
 * Copyright (C) 2008 Michael Hofmann <mh21@piware.de>                        *
 * Copyright (C) 2008 Division of Experimental Otorhinolaryngology K.U.Leuven *
 *                                                                            *
 * Original version written by Maarten Lambert.                               *
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

#ifndef _BERTHA_SRC_LIB_BERTHA_XMLLOADER_H_
#define _BERTHA_SRC_LIB_BERTHA_XMLLOADER_H_

#include "experimentdata.h"
#include "global.h"

#include <QCoreApplication>
#include <QString>

namespace bertha
{

class XmlLoaderPrivate;

/**
* Contains xml file loading tools
*/
class BERTHA_EXPORT XmlLoader
{
    Q_DECLARE_TR_FUNCTIONS(XmlLoader)
public:
    /**
    * Default constructor
    */
    XmlLoader();

    /**
    * Virtual destructor
    */
    virtual ~XmlLoader();

    /**
    * Loads a file and converts it to an ExperimentData.
    */
    ExperimentData loadFile(const QString &filePath);
    ExperimentData loadContents(const QByteArray &data);

private:
    XmlLoaderPrivate *const d;
};

}; // namespace bertha

#endif
