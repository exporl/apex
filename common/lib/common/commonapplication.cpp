/******************************************************************************
 * Copyright (C) 2011  Michael Hofmann <mh21@piware.de>                       *
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

#include "commonapplication.h"
#include "debug.h"
#include "listutils.h"
#include "paths.h"

#include <QDataStream>

namespace cmn
{

void registerCommonMetaTypes()
{
    qRegisterMetaType<cmn::StringStringList>();
    qRegisterMetaType<cmn::StringStringMap>();
    qRegisterMetaType<cmn::StringVariantList>();
    qRegisterMetaTypeStreamOperators<StringVariantList>(
        "cmn::StringVariantList");
}

// CommonCoreApplication =======================================================

CommonCoreApplication::CommonCoreApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    registerCommonMetaTypes();

    enableCoreDumps(applicationFilePath());

    Q_FOREACH (const auto &pluginPath, Paths::pluginDirectories())
        addLibraryPath(pluginPath);
}

bool CommonCoreApplication::notify(QObject *receiver, QEvent *event)
{
    try {
        return QCoreApplication::notify(receiver, event);
    } catch (const std::exception &e) {
        qFatal("Exception in QCoreApplication::notify(): %s", e.what());
    } catch (...) {
        qFatal("Unknown exception in QCoreApplication::notify()");
    }
    // Silence the compiler
    return false;
}

} // namespace cmn
