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

#ifndef _APEX_SRC_LIB_APEXTOOLS_APEXPLUGINLOADER_H_
#define _APEX_SRC_LIB_APEXTOOLS_APEXPLUGINLOADER_H_

#include "apextools/exceptions.h"

#include "common/pluginloader.h"

namespace apex
{

template <typename T>
T* createPluginCreator(const QString& name)
{
    QList<T*> modules = cmn::PluginLoader().availablePlugins<T>();

    T *creator = NULL;
    Q_FOREACH (T *module, modules) {
        if (module->availablePlugins().contains(name)) {
            creator = module;
            break;
        }
    }

    if (!creator)
        throw ApexStringException(
                cmn::PluginLoader::tr("Could not load plugin '%1'. "
                "Consult the plugin dialog under the menu help "
                "for more information")
                .arg(name));

    return creator;
}

} // namespace apex

#endif

