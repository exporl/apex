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

/** @file
 * Temporary directory management declaration.
 */

#ifndef _EXPORL_COMMON_LIB_COMMON_TEMPORARYDIRECTORY_H_
#define _EXPORL_COMMON_LIB_COMMON_TEMPORARYDIRECTORY_H_

#include "global.h"

#include <QCoreApplication>
#include <QMetaType>
#include <QString>

class QDir;
class QObject;

namespace cmn
{

/** Creates a temporary directory. It is created in the directory returned
 * by Paths::tempDirectory() and uses mkdtemp() on Unix. The same effect is
 * emulated on Windows, where each user has a private temp directory.
 *
 * @param prefix directory name prefix that will be inserted additionally to
 * the rba at the beginning and the random characters at the end
 * @return path of the created temporary directory
 *
 * @throws std::exception if the directory could not be created
 */
COMMON_EXPORT QString createTempDirectory(const QString &prefix = QString());

class TemporaryDirectoryPrivate;

/** Temporary directory creation and deletion. The directory is
 * automatically created, and it will be deleted by the destructor if the
 * directory only contains files registered with #addFile(). This class is
 * implicitly shared, i.e., it can be cheaply copied and only the last
 * instance will delete the files and directories.
 *
 * This class is threadsafe.
 */
class COMMON_EXPORT TemporaryDirectory
{
    Q_DECLARE_TR_FUNCTIONS(TemporaryDirectory)
public:
    /** Creates a new instance. The directory is created by calling
     * #createTempDirectory(). Call #addFile() to register files for
     * automatic life time management.
     *
     * @param prefix directory name prefix that will be inserted
     * additionally to the rba at the beginning and the random characters at
     * the end
     *
     * @throws std::exception if the directory could not be created
     */
    explicit TemporaryDirectory(const QString &prefix = QString());

    /** Tries to delete the temporary directory and all files registered
     * with #addFile(). No error handling is provided if the deletion fails.
     */
    ~TemporaryDirectory();

    /** Returns the temporary directory.
     *
     * @return QDir instance representing the directory
     */
    const QDir &dir() const;

    /** Returns a file name in the temporary directory.
     *
     * @param name file name
     * @return the same as dir().filePath(name)
     */
    QString file(const QString &name) const;

    /** Adds a new lifetime-controlled file. The instance will try to
     * delete the file in the destructor before removing the directory.
     * Only files in the directory are deleted. This method can be called
     * several times with the same file name, subsequent calls are ignored.
     *
     * @param name file name
     * @return full path name for the passed file name
     */
    QString addFile(const QString &name);

    /** Adds new lifetime-controlled files. The instance will try to
     * delete the files in the destructor before removing the directory.
     * Only files in the directory are deleted.
     *
     * @param names file names
     *
     * @see addFile
     */
    void addFiles(const QStringList &names);

private:
    DECLARE_PRIVATE(TemporaryDirectory)

protected:
    DECLARE_PRIVATE_DATA(TemporaryDirectory)
};

/** File and directory deleter. Automatically tries to delete a given file
 * or (empty) directory when the instance goes out of scope.
 */
class COMMON_EXPORT FileDeleter
{
public:
    /** Creates a new deleter instance. The provided filename should be the
     * absolute path of the file or directory that should be deleted once
     * the destructor is called. A directory must be empty to be deleted
     * this way.
     *
     * @param path absolute path to the file or directory to delete
     */
    explicit FileDeleter(const QString &path);

    /** Tries to delete the file or directory. No error handling is provided
     * if the deletion fails.
     */
    ~FileDeleter();

    /** Returns the file or directory name. This was provided during
     * initialization.
     *
     * @return file or directory name
     */
    QString name() const
    {
        return path;
    }

private:
    const QString path;
    Q_DISABLE_COPY(FileDeleter)
};

} // namespace cmn

#endif
