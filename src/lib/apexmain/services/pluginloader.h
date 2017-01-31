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

#ifndef _EXPORL_SRC_LIB_APEXMAIN_SERVICES_PLUGINLOADER_H_
#define _EXPORL_SRC_LIB_APEXMAIN_SERVICES_PLUGINLOADER_H_

#include "apextools/exceptions.h"
#include "apextools/global.h"

#include "apextools/services/servicemanager.h"

#include <QCoreApplication>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

namespace apex
{

    class APEX_EXPORT PluginLoader: public Service<PluginLoader> {
        Q_DECLARE_TR_FUNCTIONS(PluginLoader)

    public:
        const char* Name();

        PluginLoader();

        void insert (const QString &path, QObject *plugin);
        QList<QObject*> allAvailablePlugins() const;
        QString pluginPath (const QObject *object) const;
        QMap<QString, QString> pluginsWithErrors() const;

        template <typename T>
            QList<T*> availablePlugins()
            {
                QList<T*> result;

                Q_FOREACH (QObject * const plugin, allAvailablePlugins())
                    if (T * const casted = qobject_cast<T*> (plugin))
                        result.append (casted);

                return result;
            }

        template <typename T>
            T* createPluginCreator(const QString& name)
            {
                QList<T*> modules = availablePlugins<T>();

                T *creator = NULL;
                Q_FOREACH (T *module, modules) {
                    if (module->availablePlugins().contains(name)) {
                    //if (module->name() == name) {
                        creator = module;
                        break;
                    }
                }

                if (!creator)
                    throw ApexStringException (
                            tr ("Could not load plugin '%1'. "
                            "Consult the plugin dialog under the menu help "
                            "for more information")
                            .arg (QString (name)));

                return creator;

            }

    private:
        QList<QObject*> plugins;
        QMap<const QObject*, QString> paths;
        QMap<QString, QString> errors;
        QStringList scannedPaths;
    };

} // namespace apex

#endif

