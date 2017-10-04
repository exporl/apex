/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#include "common/paths.h"
#include "common/utils.h"

#include "apexpaths.h"
#include "apexrandom.h"
#include "apextools.h"
#include "exceptions.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QFontMetrics>
#include <QLayout>
#include <QMetaMethod>
#include <QScriptEngine>
#include <QSettings>
#include <QSize>
#include <QString>
#include <QThread>
#include <QUrl>
#include <QVariant>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#ifdef Q_OS_WIN32
#include <windows.h> // Sleep
#else
#include <unistd.h>
#endif

using namespace cmn;

namespace apex
{

// copied and modified from qobject.cpp
void ApexTools::connectSlotsByNameToPrivate(QObject *publicObject,
                                            QObject *privateObject)
{
    cmn::connectSlotsByNameToPrivate(publicObject, privateObject);
}

/**
 * Make sure the given directory name ends in '/'
 * @param
 * @return
 */
QString ApexTools::MakeDirEnd(QString p_path)
{
    if (p_path.isEmpty())
        return p_path;

    if (p_path.right(1) != "/") {
        p_path = p_path + "/";
    }

    return p_path;
}

QString ApexTools::addPrefix(const QString &file, const QString &prefix)
{
    // assets: and silence: are possible schemes
    if (!QUrl(file).scheme().isEmpty())
        return file;
    // sometimes, addPrefix is called to prepare optional filenames
    if (file.isEmpty())
        return file;
    return QFileInfo(prefix, file).filePath();
}

QString ApexTools::absolutePath(const QString &path)
{
    QFileInfo fi(path);
    return fi.absoluteFilePath();
}

QString ApexTools::boolToString(bool b)
{
    return b ? "true" : "false";
}

QString ApexTools::removeXmlTags(const QString &str)
{
    QString result = str;
    return result.remove(QRegExp("<[^<]+>"));
}

void ApexTools::milliSleep(unsigned millis)
{
#ifdef Q_OS_WIN32
    Sleep(millis);
#else
    usleep(1000 * millis);
#endif
}

static bool doRecursiveCopy(const QString &sourcePath,
                            const QString &destinationPath, bool force)

{
    QFileInfo source(sourcePath);
    QFileInfo destination(destinationPath);
    if (!destination.dir().exists()) {
        if (!destination.dir().mkpath("."))
            return false;
    }
    if (source.isDir()) {
        QDir sourceDir(source.filePath());
        Q_FOREACH (
            const QString &sourceItem,
            sourceDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
            if (!doRecursiveCopy(
                    sourceDir.filePath(sourceItem),
                    QDir(destination.filePath()).filePath(sourceItem), force))
                return false;
        }
        return true;
    } else {
        QString destinationFilePath =
            destination.isDir()
                ? QDir(destination.filePath()).filePath(source.fileName())
                : destination.filePath();
        if (force && QFile::exists(destinationFilePath))
            QFile::remove(destinationFilePath);
        return QFile::copy(source.filePath(), destinationFilePath);
    }
}

bool ApexTools::recursiveCopy(const QString &sourcePath,
                              const QString &destinationPath, bool force)
{
    QFileInfo source(sourcePath);
    QFileInfo destination(destinationPath);

    if (destination.dir().exists() && source.isDir()) {
        if (!QDir(destination.filePath()).mkpath(source.fileName()))
            return false;
        destination.setFile(
            QDir(destination.filePath()).filePath(source.fileName()));
    }
    return doRecursiveCopy(source.filePath(), destination.filePath(), force);
}

void ApexTools::InitRand(long seed)
{
    static bool srandDone = false;
    if (!srandDone) {
        if (seed == -1) {
            seed = time(0);
        }
        srand(static_cast<unsigned>(seed));
        ran1(-static_cast<long>(seed));
        srandDone = true;
    }
}

void ApexTools::ReplaceCharInString(QString &a_StringToSearch,
                                    const QChar &ac_ToReplace,
                                    const QString &ac_ReplaceWith)
{
    const unsigned nLen = a_StringToSearch.length();
    for (unsigned i = 0; i < nLen; ++i) {
        if (a_StringToSearch.at(i) == ac_ToReplace) {
            a_StringToSearch.remove(i, 1);
            a_StringToSearch.insert(i, ac_ReplaceWith);
            i += ac_ReplaceWith.length();
        }
    }
}

QString ApexTools::escapeJavascriptString(const QString &text)
{
    QString result = text;
    result.replace("\\", "\\\\");
    result.replace("\"", "\\\"");
    result.replace("'", "\\'");
    result.replace("\n", "\\n");
    result.replace("\t", "\\t");
    result.replace("\r", "\\r");
    result.replace("\f", "\\f");
    return result;
}

bool ApexTools::bQStringToBoolean(const QString &ac_sBoolean)
{
    if (ac_sBoolean == "true" || ac_sBoolean == "1")
        return true;
    else if (ac_sBoolean == "false" || ac_sBoolean == "0")
        return false;
    else
        throw(ApexStringException("cannot convert string \"" + ac_sBoolean +
                                  "\" to boolean"));
}

int ApexTools::maximumFontPointSize(const QString &text, const QSize &box,
                                    const QFont &originalFont)
{
    // qCDebug(APEX_RS) << "getting point size for text" << text;
    const int width = box.width();
    const int height = box.height();
    QFont font(originalFont);

    Q_FOREVER {
        const int pointSize = font.pointSize();
        QSize result = QFontMetrics(font).size(0, text);
        if (result.width() < width && result.height() < height)
            return pointSize;
        if (pointSize <= 1)
            return -1;
        font.setPointSize(pointSize - 1);
    }
}

void ApexTools::shrinkTillItFits(QWidget *widget, const QString &text,
                                 const QSize &border)
{
    QFont font = widget->font();

    const int fontSize =
        maximumFontPointSize(text, widget->size() - border, widget->font());

    if (fontSize > 0) {
        font.setPointSize(fontSize);
        widget->setFont(font);
    }
}

void ApexTools::expandWidgetToWindow(QWidget *widget)
{
#if defined(Q_OS_ANDROID)
    widget->layout()->setSizeConstraint(QLayout::SetMaximumSize);
    widget->showMaximized();
    widget->setMaximumSize(widget->size());
#else
    Q_UNUSED(widget);
#endif
}

QScriptValue ApexTools::QVariant2ScriptValue(QScriptEngine *e, QVariant var)
{
    if (var.isNull())
        return e->nullValue();

    switch (var.type()) {
    case QVariant::Invalid:
        return e->nullValue();
    case QVariant::Bool:
        return QScriptValue(e, var.toBool());
    case QVariant::Date:
        return e->newDate(var.toDateTime());
    case QVariant::DateTime:
        return e->newDate(var.toDateTime());
    case QVariant::Double:
        return QScriptValue(e, var.toDouble());
    case QVariant::Int:
    case QVariant::LongLong:
        return QScriptValue(e, var.toInt());
    case QVariant::String:
        return QScriptValue(e, var.toString());
    case QVariant::Time:
        return e->newDate(var.toDateTime());
    case QVariant::UInt:
        return QScriptValue(e, var.toUInt());
    default:
        qFatal("Type conversion not implemented");
    }

    // bad but better then nothing
    return QScriptValue(e, var.toString());
}

QStringList ApexTools::unique(const QStringList &list)
{
    QStringList n;
    Q_FOREACH (const QString s, list) {
        if (!n.contains(s))
            n.push_back(s);
    }
    return n;
}

QStringList ApexTools::toStringList(const QList<int> intList)
{
    QStringList result;

    Q_FOREACH (int i, intList)
        result << QString::number(i);

    return result;
}

QString ApexTools::fetchVersion()
{
    QFile commitFile(
        Paths::searchFile(QL1S("doc/commit.txt"), Paths::dataDirectories()));
    QString version;
    if (commitFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString diffstat = QString::fromLocal8Bit(commitFile.readAll());
        QStringList lines = diffstat.split(QLatin1Char('\n'));
        QString firstLine = lines.value(0);
        QString hash = firstLine.section(QLatin1Char(' '), 1, 1).left(6);
        QString dateLine =
            lines.filter(QRegExp(QLatin1String("^Date: .*"))).value(0);
        QString date = dateLine.section(QLatin1Char(' '), 1);
        version = QString::fromLatin1("(%1) - %2").arg(hash, date);
    }

    return version;
}

QString ApexTools::fetchDiffstat()
{
    QFile commitFile(
        Paths::searchFile(QL1S("doc/commit.txt"), Paths::dataDirectories()));
    QString diffstat;
    if (commitFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        diffstat = QString::fromLocal8Bit(commitFile.readAll());
    }

    return diffstat;
}

QUuid ApexTools::getApexGUID()
{
    QCoreApplication::setApplicationName(applicationName);
    QCoreApplication::setOrganizationName(organizationName);
    QCoreApplication::setOrganizationDomain(organizationDomain);

    QSettings settings;
    if (settings.contains(QSL("GUID"))) {
        return settings.value(QSL("GUID")).toUuid();
    } else {
        QUuid guid = QUuid::createUuid();
        settings.setValue(QSL("GUID"), guid);
        return guid;
    }
}
}
