/******************************************************************************
 * Copyright (C) 2017  Sanne Raymaekers <sanne.raymaekers@gmail.com>          *
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

#ifndef _APEX_SRC_LIB_APEXMAIN_STUDY_MANAGEDDIRECTORY_H_
#define _APEX_SRC_LIB_APEXMAIN_STUDY_MANAGEDDIRECTORY_H_

#include "apextools/global.h"

namespace apex
{

class ManagedDirectoryPrivate;

class APEX_EXPORT ManagedDirectory : public QObject
{
    Q_OBJECT
public:
    ManagedDirectory(QObject *parent = nullptr);
    ManagedDirectory(const QString &path, QObject *parent = nullptr);
    virtual ~ManagedDirectory();

    /* Directory info */
    QString path() const;
    bool exists();
    bool isOpen();
    /* May throw */
    QString lastCommitMessage();
    bool isClean();
    bool hasLocalRemote();
    int commitsAheadOrigin();
    int totalCommitCount();

    /* Set directory parameters */
    void setPath(const QString &path);
    void setKeyPaths(const QString &publicKeyPath,
                     const QString &privateKeyPath);
    /* May throw */
    void setRemote(const QString &url,
                   const QString &branchName = QL1S("master"));
    void setAuthor(const QString &name, const QString &email);

    /* Directory operations */
    void init(bool bare = false);
    void open();
    void close();

    /* May throw */
    void add(const QString &path);
    void add(const QStringList &paths);

    /* Optionally synchronous */
    void push(Qt::ConnectionType connectionType = Qt::QueuedConnection);
    void pull(Qt::ConnectionType connectionType = Qt::QueuedConnection);
    void fetch(Qt::ConnectionType connectionType = Qt::QueuedConnection);
    void checkout(Qt::ConnectionType connectionType = Qt::QueuedConnection);

    /* Asynchronous */
    void retryLastAction();

    static bool exists(const QString &path);
    /* May throw */
    static QStringList lsRemote(const QString &url,
                                const QString &publicKeyPath = QString(),
                                const QString &privateKeyPath = QString());

Q_SIGNALS:
    void pullDone();
    void pullFailed();

    void pushDone();
    void pushFailed();

    void retryLastActionDone();
    void progress(int received, int total);
    void error(const QString &message);

private:
    void setup();

    QScopedPointer<ManagedDirectoryPrivate> d;
};
}

#endif
