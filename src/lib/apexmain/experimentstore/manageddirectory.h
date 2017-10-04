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

#ifndef _APEX_SRC_LIB_APEXMAIN_EXPERIMENTSTORE_MANAGEDDIRECTORY_H_
#define _APEX_SRC_LIB_APEXMAIN_EXPERIMENTSTORE_MANAGEDDIRECTORY_H_

#include "apextools/global.h"

namespace apex
{

class ManagedDirectoryPrivate;

class APEX_EXPORT ManagedDirectory : public QObject
{
    Q_OBJECT

public:
    ManagedDirectory();
    ManagedDirectory(const QString &path);
    virtual ~ManagedDirectory();

    /* Directory info */
    QString path() const;
    QStringList objects() const;
    bool isClean();

    /* Set directory parameters */
    void setPath(const QString &path);
    void setKeyPaths(const QString &publicKey, const QString &privateKey);
    void setRemote(const QString &name, const QString &url,
                   const QString &branchName = QL1S("master"));
    void setAuthor(const QString &name, const QString &email);

    /* Directory operations */
    void init();
    void open();

    /* May throw */
    void add(const QString &path);
    void add(const QStringList &paths);
    void push();
    void pull();

    void retryLastAction();
    void reClone();

    static bool exists(const QString &path);

private:
    QSharedDataPointer<ManagedDirectoryPrivate> d;
};
}

#endif
