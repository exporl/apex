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

#include "temporarydirectory.h"
#include "exception.h"
#include "paths.h"
#include "random.h"

#include <QDir>
#include <QFile>
#include <QMutex>
#include <QSet>
#include <QSharedPointer>
#include <QStack>

#ifdef Q_OS_WIN32
#include <io.h>
#endif

namespace cmn
{

QString createTempDirectory(const QString &prefix)
{
#if defined(Q_OS_UNIX) && (defined(_BSD_SOURCE) ||                             \
                           _POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)
    QByteArray name = QFile::encodeName(
        QDir(Paths::tempDirectory())
            .filePath(QL1S("rba-") +
                      (!prefix.isEmpty() ? prefix + QL1C('-') : QString()) +
                      QL1S("XXXXXX")));
    if (!mkdtemp(name.data()))
        throw Exception(
            TemporaryDirectory::tr("Unable to create temporary directory"));
    return QFile::decodeName(name);
#else
    Random random;
    for (unsigned i = 0; i < 65536; ++i) {
        QString name =
            QDir(Paths::tempDirectory())
                .filePath(QL1S("rba-") +
                          (!prefix.isEmpty() ? prefix + QL1C('-') : QString()) +
                          QString::number(random.nextUInt(), 36));
        if (QDir(name).mkdir(name))
            return name;
    }
    throw Exception(
        TemporaryDirectory::tr("Unable to create temporary directory"));
#endif
}

class TemporaryDirectoryPrivate
{
public:
    TemporaryDirectoryPrivate(const QString &prefix)
        : dir(createTempDirectory(prefix))
    {
        deleters.push(
            QSharedPointer<FileDeleter>(new FileDeleter(dir.absolutePath())));
    }

    // Destroy in the right order
    ~TemporaryDirectoryPrivate()
    {
        while (!deleters.isEmpty())
            deleters.pop();
    }

    const QDir dir;
    mutable QMutex lock;
    QStack<QSharedPointer<FileDeleter>> deleters;
    QSet<QString> files;
};

// TemporaryDirectory ==========================================================

TemporaryDirectory::TemporaryDirectory(const QString &prefix)
    : dataPtr(new TemporaryDirectoryPrivate(prefix))
{
}

TemporaryDirectory::~TemporaryDirectory()
{
    delete dataPtr;
}

const QDir &TemporaryDirectory::dir() const
{
    E_D(const TemporaryDirectory);

    return d->dir;
}

QString TemporaryDirectory::file(const QString &name) const
{
    E_D(const TemporaryDirectory);

    QMutexLocker locker(&d->lock);

    return d->dir.filePath(name);
}

QString TemporaryDirectory::addFile(const QString &name)
{
    E_D(TemporaryDirectory);

    QMutexLocker locker(&d->lock);

    const QString path = QDir::cleanPath(d->dir.filePath(name));

    if (!path.startsWith(d->dir.absolutePath()))
        throw Exception(tr("Illegal filename: %1 does not start with %2")
                            .arg(path, d->dir.absolutePath()));

    if (!d->files.contains(path)) {
        d->deleters.push(QSharedPointer<FileDeleter>(new FileDeleter(path)));
        d->files.insert(path);
    }

    return path;
}

void TemporaryDirectory::addFiles(const QStringList &names)
{
    Q_FOREACH (const auto &name, names)
        addFile(name);
}

// FileDeleter =================================================================

FileDeleter::FileDeleter(const QString &path) : path(path)
{
    Q_ASSERT(QDir::isAbsolutePath(path));
}

FileDeleter::~FileDeleter()
{
    if (QDir(path).exists())
        QDir(path).rmdir(path);
    else
        QFile::remove(path);
}

} // namespace cmn
