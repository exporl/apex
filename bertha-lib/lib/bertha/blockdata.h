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

#ifndef _BERTHA_SRC_LIB_BERTHA_BLOCKDATA_H_
#define _BERTHA_SRC_LIB_BERTHA_BLOCKDATA_H_

#include "global.h"

#include <QMap>
#include <QSharedDataPointer>
#include <QString>
#include <QVariant>

namespace bertha
{

class BlockDataPrivate;

/**
 * This class represents a block.
 */
class BERTHA_EXPORT BlockData
{
public:
    /**
    * The default constructor, creates a standard block.
    */
    BlockData();

    BlockData(const QString &id, const QString &plugin);

    /**
    * Virtual destructor
    */
    virtual ~BlockData();

    BlockData(const BlockData &other);
    BlockData &operator=(const BlockData &other);

    void setId(const QString &id);
    QString id() const;

    void setPlugin(const QString &arg);
    QString plugin() const;

    void setOutputPorts(int outputPorts);
    int outputPorts() const;

    void setInputPorts(int inputPorts);
    int inputPorts() const;

    /**
    * @param name The name of the parameter
    * @param value The value of a parameter
    */
    void setParameter(const QString &name, const QVariant &value);

    /**
    * Returns the value of a parameter.
    * @param name The name of the parameter
    */
    QVariant parameter(const QString &name) const;

    /**
    * Returns map of parameter names together with their values.
    */
    QMap<QString, QVariant> parameters() const;

protected:
    QSharedDataPointer<BlockDataPrivate> d;
};

}; // namespace bertha

#endif
