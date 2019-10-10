/******************************************************************************
 * Copyright (C) 2016  Michael Hofmann <mh21@mh21.de>                         *
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

#ifndef _EXPORL_COMMON_LIB_COMMON_SLAVESERVER_H_
#define _EXPORL_COMMON_LIB_COMMON_SLAVESERVER_H_

#include "common/global.h"

#include <QCoreApplication>
#include <QList>
#include <QObject>

namespace cmn
{

class SlaveServerPrivate;

class COMMON_EXPORT SlaveServer
{
    Q_DECLARE_TR_FUNCTIONS(SlaveServer)
public:
    SlaveServer(const QString &programName, const QString &arguments,
                const QString &pythonVersion, bool verbose);
    virtual ~SlaveServer();

    // throws an exception on errors
    void call(const QByteArray &function, QByteArray *output = NULL,
              const QList<QByteArray> &parameters = QList<QByteArray>());

    void connect();
    void stop();

private:
    DECLARE_PRIVATE(SlaveServer)
protected:
    DECLARE_PRIVATE_DATA(SlaveServer)
};
}

#endif
