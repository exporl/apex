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

#include "baseapi.h"

#include "apextools/global.h"

#include "common/websocketserver.h"

using namespace cmn;

namespace apex
{
BaseApi::BaseApi(QObject *parent, const QDir &baseDir)
    : QObject(parent), baseDir(baseDir)
{
}

BaseApi::~BaseApi()
{
}

void BaseApi::registerBaseMethods(WebSocketServer *webSocketServer)
{
    webSocketServer->on(QSL("readFile"), this, QSL("readFile(QString)"));
    webSocketServer->on(QSL("absoluteFilePath"), this,
                        QSL("absoluteFilePath(QString)"));
    webSocketServer->on(QSL("writeFile"), this,
                        QSL("writeFile(QString,QString)"));
}

QString BaseApi::readFile(const QString &fileName) const
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    return file.readAll();
}

QString BaseApi::absoluteFilePath(const QString &fileName) const
{
    return baseDir.absoluteFilePath(fileName);
}

void BaseApi::writeFile(const QString &fileName, const QString &content) const
{
    // convert file URL to normal file path if necessary
    QString path = fileName;
    if (fileName.startsWith("file:///"))
        path = fileName.right(fileName.size() - 8);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream streamFileOut(&file);
        streamFileOut.setCodec("UTF-8");
        streamFileOut << content;
        streamFileOut.flush();
        file.close();
    } else
        qCWarning(APEX_RS, "Could not access file path: %s", qPrintable(path));
}
} // namespace apex
