/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
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

#include "apextools/global.h"

#include "xmlpluginapi.h"

#include <QDebug>
#include <QDir>

namespace apex
{

QString XMLPluginAPI::version()
{
    return "This is a version string";
}

QStringList XMLPluginAPI::files(const QString &path)
{
    QString p(path);

    QDir d(p);
    if (d.exists()) // there is no wildcard specified
        return d.entryList();
    else {
        // assume the last part of the path is the wildcard
        QStringList parts(QDir::fromNativeSeparators(p).split("/"));
        QString wildcard(parts.at(parts.size() - 1));

        if (p.length() == wildcard.length())
            d.setPath(d.currentPath());
        else
            d.setPath(p.left(p.length() - wildcard.length()));
        d.setNameFilters(QStringList() << wildcard);

        qCDebug(APEX_RS) << d.entryList();

        return d.entryList();
    }
}

QString XMLPluginAPI::readAll(const QString &path)
{
    QFile f(path);
    if (!f.open(QFile::ReadOnly)) {
        addError(tr("Could not open file %1").arg(path));
        return QString();
    }
    QTextStream in(&f);
    in.setCodec("UTF-8");
    return in.readAll();
}

double XMLPluginAPI::stimulusDuration(const QString &path)
{
    streamapp::InputWaveFile p;
    RETURN_VAL_IF_FAIL(p.mp_bOpen(path), 0.0);
    return p.mf_lTotalSamples() / (double)p.mf_lSampleRate();
}

QString XMLPluginAPI::path(const QString &s)
{
    return QFileInfo(s).path();
}

QString XMLPluginAPI::stripPath(const QString &s)
{
    WARN_DEPRECATED("use path() instead of stripPath()");
    return path(s) + QSL("/");
}

QString XMLPluginAPI::absoluteFilePath(const QString &path)
{
    return QFileInfo(path).absoluteFilePath();
}

void XMLPluginAPI::addWarning(const QString &warning)
{
    qCWarning(APEX_RS, "%s",
              qPrintable(QSL("%1: %2").arg("XML Plugin script", warning)));
}

void XMLPluginAPI::addError(const QString &warning)
{
    qCCritical(APEX_RS, "%s",
               qPrintable(QSL("%1: %2").arg("XML Plugin script", warning)));
}
}
