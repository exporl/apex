/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
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

#include "apextools/services/paths.h"

#include "pluginloader.h"

#include <QCoreApplication>
#include <QDir>
#include <QMap>
#include <QPluginLoader>

namespace apex
{

PluginLoader::PluginLoader()
{
    Q_FOREACH (QObject *plugin, QPluginLoader::staticInstances())
        insert (QLatin1String ("statically linked"), plugin);

    Q_FOREACH (const QString &pluginDirectory, Paths::Get().GetBinaryPluginPaths()) {
        const QDir dir (pluginDirectory);

        if (scannedPaths.contains( dir.absolutePath()))
            return;
        else
            scannedPaths.push_back( dir.absolutePath());

        QStringList nameFilters;
#if defined(Q_OS_UNIX)
        nameFilters << QLatin1String ("lib*.so");
#elif defined(Q_OS_WIN32)
        nameFilters << QLatin1String ("*.dll");
#endif

        qCDebug(APEX_RS, "Pluginloader: scanning directory %s", qPrintable(dir.path()));
        Q_FOREACH (const QString &fileName, dir.entryList (nameFilters,
                QDir::Files)) {
            if (fileName.toLower().startsWith(QLatin1String("qt")))
                continue;
            const QString filePath (dir.absoluteFilePath (fileName));
            QPluginLoader loader (filePath);
            QObject *plugin = loader.instance();
            if (plugin)
                insert (filePath, plugin);
            else errors.insert (filePath, loader.errorString());
        }
    }
    //qCDebug(APEX_RS, "Build key: %s", QT_BUILD_KEY);
}

void PluginLoader::insert (const QString &path, QObject *plugin)
{
    plugins.append (plugin);
    paths.insert (plugin, path);
}

QList<QObject*> PluginLoader::allAvailablePlugins() const
{
    return plugins;
}

QString PluginLoader::pluginPath (const QObject *object) const
{
    return paths.value (object);
}

QMap<QString, QString> PluginLoader::pluginsWithErrors() const
{
    return errors;
}

const char* PluginLoader::Name() {
    return "PluginLoader";
}


} // namespace apex

