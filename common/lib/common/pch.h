/******************************************************************************
 * Copyright (C) 2014  Michael Hofmann <mh21@mh21.de>                         *
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

/** @file
 * Precompiled header.
 */

#if defined __cplusplus
// Add C++ includes here

// These have to go first to have the Q_OS_* and Q_CC_* defines
#include <QByteArray>
#include <QCoreApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLibrary>
#include <QList>
#include <QMap>
#include <QMetaProperty>
#include <QMetaType>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QPluginLoader>
#include <QReadWriteLock>
#include <QSet>
#include <QSettings>
#include <QSharedPointer>
#include <QStack>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QVariant>
#include <QWaitCondition>
#include <QXmlDefaultHandler>
#include <QtGlobal>

#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#endif

// Add C includes here
#ifdef Q_OS_UNIX
#include <signal.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

#ifdef Q_OS_WIN32
#include <CRTDBG.h>
#include <io.h>
#include <shlobj.h>
#include <windows.h>
#endif

#ifdef Q_OS_ANDROID
#include <android/log.h>
#endif
