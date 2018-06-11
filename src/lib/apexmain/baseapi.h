/******************************************************************************
 * Copyright (C) 2017 Sanne Raymaekers <sanne.raymaekers@gmail.com>           *
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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_BASEAPI_H_
#define _EXPORL_SRC_LIB_APEXMAIN_BASEAPI_H_

#include <QDir>
#include <QObject>

namespace cmn
{
class WebSocketServer;
}

namespace apex
{
class BaseApi : public QObject
{
    Q_OBJECT
public:
    BaseApi(QObject *parent = nullptr, const QDir &baseDir = QDir::current());
    virtual ~BaseApi();

    void registerBaseMethods(cmn::WebSocketServer *webSocketServer);

public Q_SLOTS:
    QString readFile(const QString &fileName) const;
    QString absoluteFilePath(const QString &fileName) const;

protected:
    QDir baseDir;
};
}

#endif
